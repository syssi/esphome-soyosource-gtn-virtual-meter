#include "soyosource_display.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace soyosource_display {

static const char *const TAG = "soyosource_display";

static const uint8_t QUERY_STATUS_REQUEST = 0x01;
static const uint8_t QUERY_STATUS_RESPONSE = 0x03;
static const uint8_t QUERY_SETTINGS_REQUEST = 0x03;
static const uint8_t QUERY_SETTINGS_RESPONSE = 0x01;

static const uint8_t OPERATION_STATUS_SIZE = 13;
static const char *const OPERATION_STATUS[OPERATION_STATUS_SIZE] = {
    "Normal",                  // 0x00
    "Startup",                 // 0x01
    "Standby",                 // 0x02
    "Startup aborted",         // 0x03
    "Error or battery mode?",  // 0x04
    "Unknown",                 // 0x05
    "Unknown",                 // 0x06
    "Unknown",                 // 0x07
    "Unknown",                 // 0x08
    "Unknown",                 // 0x09
    "AC input low",            // 0x10 or 0x0A?!
    "Unknown",                 // 0x11
    "Unknown",                 // 0x12
};

static const uint8_t OPERATION_MODES_SIZE = 15;
static const char *const OPERATION_MODES[OPERATION_MODES_SIZE] = {
    "Unknown",                 // 0x00
    "Battery Constant Power",  // 0x01
    "Unknown",                 // 0x02
    "Unknown",                 // 0x03
    "Unknown",                 // 0x04
    "Unknown",                 // 0x05
    "PV",                      // 0x06
    "Unknown",                 // 0x07
    "Unknown",                 // 0x08
    "Battery Limit",           // 0x09
    "Unknown",                 // 0x0A
    "Unknown",                 // 0x0B
    "Unknown",                 // 0x0C
    "Unknown",                 // 0x0D
    "PV Limit",                // 0x0E
};

void SoyosourceDisplay::loop() {
  const uint32_t now = millis();

  if (now - this->last_byte_ > 50) {
    this->rx_buffer_.clear();
    this->last_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_soyosource_display_byte_(byte)) {
      this->last_byte_ = now;
    } else {
      this->rx_buffer_.clear();
    }
  }
}

uint8_t chksum(const uint8_t data[], const uint8_t len) {
  uint8_t checksum = 0xFF;
  for (uint8_t i = 1; i < len; i++) {
    checksum = checksum - data[i];
  }
  return checksum;
}

bool SoyosourceDisplay::parse_soyosource_display_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];
  uint16_t frame_len = 15;

  // Supported responses
  //
  // Status response:    0xA6 0x03 0x84 0x91 0x40 0x01 0xC5 0x00 0xDB 0x00 0xF7 0x63 0x02 0xBC 0xFE
  // Settings response:  0xA6 0x01 0x72 0x93 0x40 0xD4 0x30 0x2C 0x2B 0x00 0xFA 0x64 0x5A 0x03 0xA3
  //                     addr func ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^  crc
  //                                                         data
  if (at == 0)
    return true;

  if (raw[0] != 0xA6) {
    ESP_LOGW(TAG, "Invalid header.");

    // return false to reset buffer
    return false;
  }

  // Byte 0...14
  if (at < frame_len - 1)
    return true;

  uint8_t function = raw[1];

  uint8_t computed_crc = chksum(raw, frame_len - 1);
  uint8_t remote_crc = raw[frame_len - 1];
  if (computed_crc != remote_crc) {
    ESP_LOGW(TAG, "SoyosourceDisplay CRC Check failed! %02X != %02X", computed_crc, remote_crc);
    return false;
  }

  ESP_LOGVV(TAG, "RX <- %s", format_hex_pretty(raw, at + 1).c_str());

  std::vector<uint8_t> data(this->rx_buffer_.begin() + 2, this->rx_buffer_.begin() + frame_len - 1);

  this->on_soyosource_display_data_(function, data);

  // return false to reset buffer
  return false;
}

void SoyosourceDisplay::on_soyosource_display_data_(const uint8_t &function, const std::vector<uint8_t> &data) {
  if (data.size() != 12) {
    ESP_LOGW(TAG, "Invalid size for soyosource display status packet!");
    return;
  }

  switch (function) {
    case QUERY_STATUS_RESPONSE:
      this->on_status_data_(data);
      break;
    case QUERY_SETTINGS_RESPONSE:
      this->on_settings_data_(data);
      break;
    default:
      ESP_LOGW(TAG, "Unhandled response received: %s", format_hex_pretty(&data.front(), data.size()).c_str());
  }
}

void SoyosourceDisplay::on_status_data_(const std::vector<uint8_t> &data) {
  auto soyosource_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  // 0x84: Unknown
  ESP_LOGD(TAG, "Unknown byte from status response: %02X", data[0]);

  // 0x91: 1x: BattConst, 6x: PVMode, 9x: BattLimit, Ex: PVLimit
  ESP_LOGD(TAG, "Operation mode (raw): %02X", data[1]);
  uint8_t raw_operation_mode = data[1] >> 4;
  this->publish_state_(this->operation_mode_id_sensor_, raw_operation_mode);

  if (raw_operation_mode < OPERATION_MODES_SIZE) {
    this->publish_state_(this->operation_mode_text_sensor_, OPERATION_MODES[raw_operation_mode]);
  } else {
    this->publish_state_(this->operation_mode_text_sensor_, "Unknown");
  }

  // 0x40: Operation status
  ESP_LOGD(TAG, "Operation status (raw): %02X", data[2]);
  uint8_t raw_operation_status = data[2] & 15;
  this->publish_state_(this->operation_status_id_sensor_, raw_operation_status);
  if (raw_operation_status < OPERATION_STATUS_SIZE) {
    this->publish_state_(this->operation_status_text_sensor_, OPERATION_STATUS[raw_operation_status]);
  } else {
    this->publish_state_(this->operation_status_text_sensor_, "Unknown");
  }

  // 0x01 0xC5: Battery voltage
  uint16_t raw_battery_voltage = soyosource_get_16bit(3);
  float battery_voltage = raw_battery_voltage * 0.1f;
  this->publish_state_(this->battery_voltage_sensor_, battery_voltage);

  // 0x00 0xDB: Battery current
  uint16_t raw_battery_current = soyosource_get_16bit(5);
  float battery_current = raw_battery_current * 0.1f;
  float battery_power = battery_voltage * battery_current;
  this->publish_state_(this->battery_current_sensor_, battery_current);
  this->publish_state_(this->battery_power_sensor_, battery_power);

  // 0x00 0xF7: Grid voltage
  uint16_t raw_ac_voltage = soyosource_get_16bit(7);
  float ac_voltage = raw_ac_voltage * 1.0f;
  this->publish_state_(this->ac_voltage_sensor_, ac_voltage);

  // 0x63: Grid frequency
  float ac_frequency = data[9] * 0.5f;
  this->publish_state_(this->ac_frequency_sensor_, ac_frequency);

  // 0x02 0xBC
  float temperature = (soyosource_get_16bit(10) - 200) * 0.1f;  // may be - 300?
  this->publish_state_(this->temperature_sensor_, temperature);
  this->publish_state_(this->fan_running_binary_sensor_, (bool) (temperature >= 45.0));
}

void SoyosourceDisplay::on_settings_data_(const std::vector<uint8_t> &data) {
  auto soyosource_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  ESP_LOGI(TAG, "Settings:");

  // 0x72
  ESP_LOGI(TAG, "  Unknown (byte 0): %02X", data[0]);

  // 0x93: 1x: BattConst, 6x: PVMode, 9x: BattLimit, Ex: PVLimit
  ESP_LOGD(TAG, "  Operation mode (raw): %02X", data[1]);
  uint8_t raw_operation_mode = data[1] >> 4;
  if (raw_operation_mode < OPERATION_MODES_SIZE) {
    ESP_LOGI(TAG, "  Operation mode: %s (%d)", OPERATION_MODES[raw_operation_mode], raw_operation_mode);
  } else {
    ESP_LOGI(TAG, "  Operation mode: Unknown (%d)", raw_operation_mode);
  }

  // 0x40
  ESP_LOGD(TAG, "  Operation status (raw): %02X", data[2]);
  uint8_t raw_operation_status = data[2] & 15;
  if (raw_operation_status < OPERATION_STATUS_SIZE) {
    ESP_LOGI(TAG, "  Operation status: %s (%d)", OPERATION_STATUS[raw_operation_status], raw_operation_status);
    this->publish_state_(this->operation_status_text_sensor_, OPERATION_STATUS[raw_operation_status]);
  } else {
    ESP_LOGI(TAG, "  Operation status: Unknown (%d)", raw_operation_status);
  }

  // 0xD4 0x30
  ESP_LOGI(TAG, "  Unknown (byte 3 and byte 4): %02X %02X", data[3], data[4]);

  // 0x2C: Starting voltage (44 V)
  ESP_LOGI(TAG, "  Starting voltage: %d V", data[5]);

  // 0x2B: Shutdown voltage (43 V)
  ESP_LOGI(TAG, "  Stutdown voltage: %d V", data[6]);

  // 0x00 0xFA: Grid voltage
  ESP_LOGI(TAG, "  Grid voltage: %.0f V", (float) soyosource_get_16bit(7));

  // 0x64: Grid frequency
  ESP_LOGI(TAG, "  Grid frequency: %.1f Hz", (float) data[9] * 0.5f);

  // 0x5A: Battery output power
  ESP_LOGI(TAG, "  Constant power mode power: %d W", data[10] * 10);

  // 0x03: Delay in seconds
  ESP_LOGI(TAG, "  Start delay: %d s", data[11]);
}

void SoyosourceDisplay::dump_config() { ESP_LOGCONFIG(TAG, "SoyosouceDisplay:"); }
float SoyosourceDisplay::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void SoyosourceDisplay::update() { this->query_data_(QUERY_STATUS_REQUEST); }

void SoyosourceDisplay::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void SoyosourceDisplay::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void SoyosourceDisplay::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

void SoyosourceDisplay::query_data_(uint8_t function) {
  uint8_t frame[12];

  frame[0] = 0x55;      // header
  frame[1] = function;  // function (0x01: status, 0x03: settings)
  frame[2] = 0x00;
  frame[3] = 0x00;
  frame[4] = 0x00;
  frame[5] = 0x00;
  frame[6] = 0x00;
  frame[7] = 0x00;
  frame[8] = 0x00;
  frame[9] = 0x00;
  frame[10] = 0x00;
  frame[11] = chksum(frame, 11);

  this->write_array(frame, 12);
  this->flush();
}

}  // namespace soyosource_display
}  // namespace esphome
