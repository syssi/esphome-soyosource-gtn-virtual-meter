#include "soyosource_inverter.h"
#include "esphome/core/log.h"

namespace esphome {
namespace soyosource_inverter {

static const char *const TAG = "soyosource_inverter";

void SoyosourceInverter::on_soyosource_modbus_data(const std::vector<uint8_t> &data) {
  if (data.size() != 10) {
    ESP_LOGW(TAG, "Invalid size for soyosource status packet!");
    return;
  }

  ESP_LOGD(TAG, "Status frame: %s", hexencode(&data.front(), data.size()).c_str());

  auto soyosource_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  float operation_mode_id = (float) data[0];

  uint16_t raw_battery_voltage = soyosource_get_16bit(1);
  float battery_voltage = raw_battery_voltage * 0.1f;

  uint16_t raw_battery_current = soyosource_get_16bit(3);
  float battery_current = raw_battery_current * 0.1f;
  float battery_power = battery_voltage * battery_current;

  uint16_t raw_ac_voltage = soyosource_get_16bit(5);
  float ac_voltage = raw_ac_voltage * 1.0f;

  float ac_frequency = data[7] * 0.5f;
  float temperature = (data[8] * 10.0f) + (data[9] * 0.1f);

  if (battery_power > 2500 || ac_voltage > 300 || ac_frequency > 100 || temperature > 200) {
    ESP_LOGW(TAG, "Frame dropped because of unlikely measurements!");
    return;
  }

  // https://secondlifestorage.com/index.php?threads/limiter-inverter-with-rs485-load-setting.7631/
  // Soyosource status frame:          0x23 0x01 0x01 0x00 0x00 0x01 0xDB 0x00 0xA1 0x00 0xDD 0x64 0x02 0xCA 0x01
  // Full frame bytes:                   0    1    2    3    4    5    6    7    8    9   10   11   12   13   14
  // Data bytes:                                             0    1    2    3    4    5    6    7    8    9   CRC
  //
  // Response of my broken soyosource: 0x23 0x01 0x01 0x00 0x04 0x02 0xBD 0x00 0x46 0x00 0xEF 0x64 0x02 0x43 0x20
  //                                     0    1    2    3    4    5    6    7    8    9   10   11   12   13   14
  // Data bytes:                                             0    1    2    3    4    5    6    7    8    9   CRC
  //
  //
  // Data byte 0:          0x04 (Operation mode)    0x00 (Normal), 0x01 (Startup), 0x02 (Standby), 0x04 (Error?)
  // Data byte 1...2: 0x01 0xDB (Battery voltage)
  // Data byte 3...4: 0x00 0xA1 (Battery current)
  // Data byte 5...6: 0x00 0xDD (AC voltage)
  // Data byte 7:          0x64 (AC frequency / 2)
  // Data byte 8...9: 0x02 0xCA (Temperature / 10.0 + 2 * 10)

  if (this->operation_mode_id_sensor_ != nullptr)
    this->operation_mode_id_sensor_->publish_state(operation_mode_id);

  if (this->battery_voltage_sensor_ != nullptr)
    this->battery_voltage_sensor_->publish_state(battery_voltage);

  if (this->battery_current_sensor_ != nullptr)
    this->battery_current_sensor_->publish_state(battery_current);

  if (this->battery_power_sensor_ != nullptr)
    this->battery_power_sensor_->publish_state(battery_power);

  if (this->ac_voltage_sensor_ != nullptr)
    this->ac_voltage_sensor_->publish_state(ac_voltage);

  if (this->ac_frequency_sensor_ != nullptr)
    this->ac_frequency_sensor_->publish_state(ac_frequency);

  if (this->temperature_sensor_ != nullptr)
    this->temperature_sensor_->publish_state(temperature);
}

void SoyosourceInverter::update() { this->query_status(); }

void SoyosourceInverter::dump_config() {
  ESP_LOGCONFIG(TAG, "SoyosourceInverter:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("", "Operation Mode ID", this->operation_mode_id_sensor_);
  LOG_SENSOR("", "Battery Voltage", this->battery_voltage_sensor_);
  LOG_SENSOR("", "Battery Current", this->battery_current_sensor_);
  LOG_SENSOR("", "Battery Power", this->battery_power_sensor_);
  LOG_SENSOR("", "AC Voltage", this->ac_voltage_sensor_);
  LOG_SENSOR("", "AC Frequency", this->ac_frequency_sensor_);
  LOG_SENSOR("", "Temperature", this->temperature_sensor_);
}

}  // namespace soyosource_inverter
}  // namespace esphome
