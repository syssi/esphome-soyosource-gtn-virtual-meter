#include "soyosource_inverter.h"
#include "esphome/core/log.h"

namespace esphome {
namespace soyosource_inverter {

static const char *const TAG = "soyosource_inverter";

void SoyosourceInverter::on_soyosource_modbus_data(const std::vector<uint8_t> &data) {
  if (data.size() != 14 + 1 - 5) {
    ESP_LOGW(TAG, "Invalid size for soyosource status packet!");
    return;
  }

  ESP_LOGW(TAG, "Soyosource status packet received.");

  auto soyosource_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  uint16_t raw_battery_voltage = soyosource_get_16bit(0);
  float battery_voltage = raw_battery_voltage * 0.1f;

  uint16_t raw_battery_current = soyosource_get_16bit(2);
  float battery_current = raw_battery_current * 0.1f;

  uint16_t raw_ac_voltage = soyosource_get_16bit(4);
  float ac_voltage = raw_ac_voltage * 1.0f;

  float ac_frequency = data[6] * 0.5f;
  float temperature = (data[7] * 10.0f) + (data[8] * 0.1f);
  float error_bits = data[9] * 1.0f;

  // https://secondlifestorage.com/index.php?threads/limiter-inverter-with-rs485-load-setting.7631/
  // Soyosource status frame: 0x23 0x01 0x01 0x00 0x00 0x01 0xDB 0x00 0xA1 0x00 0xDD 0x64 0x02 0xCA 0x01
  // Full frame bytes:          0    1    2    3    4    5    6    7    8    9   10    11  12   13   14
  // Data bytes:                                         0    1    2    3    4    5     6   7    8    9
  //
  // Data byte 0...1: 0x01 0xDB =  (Battery voltage)
  // Data byte 2...3: 0x00 0xA1 (Battery current)
  // Data byte 4...5: 0x00 0xDD (AC voltage)
  // Data byte 6: 0x64 (AC frequency / 2)
  // Data byte 7...8: 0x02 0xCA (Temperature / 10.0 + 2 * 10)
  // Date byte 9: 0x01 (Error bitmask?)

  if (this->battery_voltage_sensor_ != nullptr)
    this->battery_voltage_sensor_->publish_state(battery_voltage);

  if (this->battery_current_sensor_ != nullptr)
    this->battery_current_sensor_->publish_state(battery_current);

  if (this->battery_power_sensor_ != nullptr)
    this->battery_power_sensor_->publish_state(battery_voltage * battery_current);

  if (this->ac_voltage_sensor_ != nullptr)
    this->ac_voltage_sensor_->publish_state(ac_voltage);

  if (this->ac_frequency_sensor_ != nullptr)
    this->ac_frequency_sensor_->publish_state(ac_frequency);

  if (this->temperature_sensor_ != nullptr)
    this->temperature_sensor_->publish_state(temperature);

  if (this->error_bits_sensor_ != nullptr)
    this->error_bits_sensor_->publish_state(error_bits);
}

void SoyosourceInverter::dump_config() {
  ESP_LOGCONFIG(TAG, "SoyosourceInverter:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("", "Battery Voltage", this->battery_voltage_sensor_);
  LOG_SENSOR("", "Battery Current", this->battery_current_sensor_);
  LOG_SENSOR("", "Battery Power", this->battery_power_sensor_);
  LOG_SENSOR("", "AC Voltage", this->ac_voltage_sensor_);
  LOG_SENSOR("", "AC Frequency", this->ac_frequency_sensor_);
  LOG_SENSOR("", "Temperature", this->temperature_sensor_);
  LOG_SENSOR("", "Error Bits", this->error_bits_sensor_);
}

}  // namespace soyosource_inverter
}  // namespace esphome
