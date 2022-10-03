#include "soyosource_display.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace soyosource_display {

static const char *const TAG = "soyosource_display";

static const uint8_t SOF_REQUEST = 0x55;
static const uint8_t SOF_SOYO_RESPONSE = 0xA6;
static const uint8_t SOF_MS51_RESPONSE = 0x5A;

static const uint8_t SOF_SOYO_RESPONSE_LEN = 15;
static const uint8_t SOF_MS51_RESPONSE_LEN = 17;

static const uint8_t STATUS_COMMAND = 0x01;
static const uint8_t SETTINGS_COMMAND = 0x03;
static const uint8_t REBOOT_COMMAND = 0x11;
static const uint8_t WRITE_SETTINGS_COMMAND = 0x0B;

static const uint8_t ERRORS_SIZE = 8;
static const char *const ERRORS[ERRORS_SIZE] = {
    "Unknown (Bit 1)",      // 0000 0001
    "DC voltage too low",   // 0000 0010
    "DC voltage too high",  // 0000 0100
    "AC voltage too high",  // 0000 1000
    "AC voltage too low",   // 0001 0000
    "Unknown (Bit 6)",      // 0010 0000
    "Limiter connected",    // 0100 0000
    "Unknown (Bit 8)",      // 1000 0000
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
  uint16_t frame_len = SOF_SOYO_RESPONSE_LEN;
  uint8_t function_pos = 3;

  // Supported Soyosource responses
  //
  // Status request        >>> 0x55 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFE
  // Status response       <<< 0xA6 0x00 0x00 0xD1 0x02 0x00 0x00 0x00 0x00 0x00 0xFB 0x64 0x02 0x0D 0xBE
  //
  // Settings request      >>> 0x55 0x03 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFC
  // Settings response     <<< 0xA6 0x00 0x00 0xD3 0x02 0xD4 0x30 0x30 0x2D 0x00 0xFB 0x64 0x4B 0x06 0x19
  //
  if (at == 0)
    return true;

  if (raw[0] != SOF_SOYO_RESPONSE && raw[0] != SOF_MS51_RESPONSE) {
    ESP_LOGVV(TAG, "Invalid header");

    // return false to reset buffer
    return false;
  }

  // Supported MS51 responses
  //
  // Status request        >>> 0x55 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFE
  // Status response       <<< 0x5A 0x01 0xD1 0x02 0x00 0x00 0x00 0x00 0x00 0xE7 0x32 0x00 0x00 0x00 0x00 0x19 0xF9
  //
  // Settings request     >>> 0x55 0x03 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFC
  // Settings response    <<< 0x5A 0x01 0xD3 0x02 0xD4 0x30 0x31 0x2F 0x00 0xE7 0x64 0x5A 0x00 0x06 0x37 0x5A 0x89
  //
  if (raw[0] == SOF_MS51_RESPONSE) {
    frame_len = SOF_MS51_RESPONSE_LEN;
    function_pos = 2;
  }

  // Byte 0...14
  if (at < frame_len - 1)
    return true;

  uint8_t function = raw[function_pos] & 15;

  uint8_t computed_crc = chksum(raw, frame_len - 1);
  uint8_t remote_crc = raw[frame_len - 1];
  if (computed_crc != remote_crc) {
    ESP_LOGW(TAG, "SoyosourceDisplay CRC Check failed! %02X != %02X", computed_crc, remote_crc);
    return false;
  }

  std::vector<uint8_t> data(this->rx_buffer_.begin(), this->rx_buffer_.begin() + frame_len - 1);

  this->on_soyosource_display_data_(function, data);

  // return false to reset buffer
  return false;
}

void SoyosourceDisplay::on_soyosource_display_data_(const uint8_t &function, const std::vector<uint8_t> &data) {
  if (data.size() != SOF_SOYO_RESPONSE_LEN - 1 && data.size() != SOF_MS51_RESPONSE_LEN - 1) {
    ESP_LOGW(TAG, "Invalid frame size!");
    return;
  }

  uint8_t response_source = data[0];
  if (response_source == SOF_MS51_RESPONSE) {
    switch (function) {
      case STATUS_COMMAND:
        this->on_ms51_status_data_(data);
        this->send_command(SETTINGS_COMMAND);
        return;
      case SETTINGS_COMMAND:
        this->on_ms51_settings_data_(data);
        return;
      case 0x00:
        ESP_LOGE(TAG, "No data from the Soyosource inverter received yet. This should never happen if the dongle is "
                      "plugged into the inverter");
        return;
    }
  }

  if (response_source == SOF_SOYO_RESPONSE) {
    switch (function) {
      case STATUS_COMMAND:
        this->on_soyosource_status_data_(data);
        this->send_command(SETTINGS_COMMAND);
        return;
      case SETTINGS_COMMAND:
        this->on_soyosource_settings_data_(data);
        return;
    }
  }

  ESP_LOGW(TAG, "Unhandled response received: %s", format_hex_pretty(&data.front(), data.size()).c_str());
}

void SoyosourceDisplay::on_ms51_status_data_(const std::vector<uint8_t> &data) {
  auto soyosource_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  ESP_LOGI(TAG, "Status frame received");

  // Soyosource status: 0xA6 0x02 0xEA 0x91 0x40 0x01 0xC5 0x00 0x32 0x00 0xF7 0x64 0x02 0x12 0xDB
  // MS5 status:             0x5A 0x01 0x91 0x40 0x01 0xC5 0x00 0x32 0x00 0xF7 0x32 0x00 0xCA 0x00 0x00 0x17 0x2B

  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     1   0x5A                   Header
  // 1     1   0x01
  ESP_LOGD(TAG, "Unknown (raw): %02X (always 0x01?)", data[1]);

  // 2     1   0x91                   Operation mode (High nibble), Frame function (Low nibble)
  ESP_LOGV(TAG, "Operation mode (raw): %02X", data[2]);
  uint8_t raw_operation_mode = data[2] >> 4;
  this->publish_state_(this->operation_mode_id_sensor_, raw_operation_mode);
  this->publish_state_(this->operation_mode_text_sensor_, this->operation_mode_to_string_(raw_operation_mode));

  // 3     1   0x40                   Error and status bitmask
  ESP_LOGV(TAG, "Error and status bitmask (raw): %02X", data[3]);
  uint8_t raw_status_bitmask = data[3] & ~(1 << 6);
  this->publish_state_(this->limiter_connected_binary_sensor_, (bool) (data[3] & (1 << 6)));
  this->publish_state_(this->operation_status_id_sensor_, (raw_status_bitmask == 0x00) ? 0 : 2);
  this->publish_state_(this->operation_status_text_sensor_, (raw_status_bitmask == 0x00) ? "Normal" : "Standby");
  this->publish_state_(this->error_bitmask_sensor_, (float) raw_status_bitmask);
  this->publish_state_(this->errors_text_sensor_, this->error_bits_to_string_(raw_status_bitmask));

  // 4     2   0x01 0xC5              Battery voltage
  uint16_t raw_battery_voltage = soyosource_get_16bit(4);
  float battery_voltage = raw_battery_voltage * 0.1f;
  this->publish_state_(this->battery_voltage_sensor_, battery_voltage);

  // 6     2   0x00 0x32              Battery current
  uint16_t raw_battery_current = soyosource_get_16bit(6);
  float battery_current = raw_battery_current * 0.1f;
  float battery_power = battery_voltage * battery_current;
  this->publish_state_(this->battery_current_sensor_, battery_current);
  this->publish_state_(this->battery_power_sensor_, battery_power);

  // 8     2   0x00 0xF7              Grid voltage
  uint16_t raw_ac_voltage = soyosource_get_16bit(8);
  float ac_voltage = raw_ac_voltage * 1.0f;
  this->publish_state_(this->ac_voltage_sensor_, ac_voltage);

  // 10    1   0x32                   Grid frequency       1.0         Hz          50 Hz
  this->publish_state_(this->ac_frequency_sensor_, data[10]);

  // 11    2   0x00 0xCA              Output Power         1.0         W           202 W
  this->publish_state_(this->output_power_sensor_, soyosource_get_16bit(11) * 1.0f);

  // 13    2   0x00 0x00              Total energy         0.1         kWh         00.0 kWh (wrap around at 6553.5 kWh)
  this->publish_state_(this->total_energy_sensor_, soyosource_get_16bit(13) * 0.1f);

  // 15    1   0x17                   Temperature          1.0         °C          23 °C
  uint8_t temperature = data[15];
  this->publish_state_(this->temperature_sensor_, temperature);
  this->publish_state_(this->fan_running_binary_sensor_, (bool) (temperature >= 45.0));
}

void SoyosourceDisplay::on_soyosource_status_data_(const std::vector<uint8_t> &data) {
  auto soyosource_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  ESP_LOGI(TAG, "Status frame received");

  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     1   0xA6                   Header
  // 1     2   0x00 0x84              Output Power         1.0         W           132 W
  this->publish_state_(this->output_power_sensor_, soyosource_get_16bit(1) * 1.0f);

  // 3     1   0x91                   Operation mode (High nibble), Frame function (Low nibble)
  //                                                                0x01: Status frame
  ESP_LOGV(TAG, "  Operation mode (raw): %02X", data[3]);
  uint8_t raw_operation_mode = data[3] >> 4;
  this->publish_state_(this->operation_mode_id_sensor_, raw_operation_mode);
  this->publish_state_(this->operation_mode_text_sensor_, this->operation_mode_to_string_(raw_operation_mode));

  // 4     1   0x40                   Error and status bitmask
  ESP_LOGV(TAG, "  Error and status bitmask (raw): %02X", data[4]);
  uint8_t raw_status_bitmask = data[4] & ~(1 << 6);
  this->publish_state_(this->limiter_connected_binary_sensor_, (bool) (data[4] & (1 << 6)));
  this->publish_state_(this->operation_status_id_sensor_, (raw_status_bitmask == 0x00) ? 0 : 2);
  this->publish_state_(this->operation_status_text_sensor_, (raw_status_bitmask == 0x00) ? "Normal" : "Standby");
  this->publish_state_(this->error_bitmask_sensor_, (float) raw_status_bitmask);
  this->publish_state_(this->errors_text_sensor_, this->error_bits_to_string_(raw_status_bitmask));

  // 5     2   0x01 0xC5              Battery voltage
  uint16_t raw_battery_voltage = soyosource_get_16bit(5);
  float battery_voltage = raw_battery_voltage * 0.1f;
  this->publish_state_(this->battery_voltage_sensor_, battery_voltage);

  // 7     2   0x00 0xDB              Battery current
  uint16_t raw_battery_current = soyosource_get_16bit(7);
  float battery_current = raw_battery_current * 0.1f;
  float battery_power = battery_voltage * battery_current;
  this->publish_state_(this->battery_current_sensor_, battery_current);
  this->publish_state_(this->battery_power_sensor_, battery_power);

  // 9     2   0x00 0xF7              Grid voltage
  uint16_t raw_ac_voltage = soyosource_get_16bit(9);
  float ac_voltage = raw_ac_voltage * 1.0f;
  this->publish_state_(this->ac_voltage_sensor_, ac_voltage);

  // 11     1   0x63                   Grid frequency
  float ac_frequency = data[11] * 0.5f;
  this->publish_state_(this->ac_frequency_sensor_, ac_frequency);

  // 12    2   0x02 0xBC              Temperature
  float temperature = (soyosource_get_16bit(12) - 300) * 0.1f;
  this->publish_state_(this->temperature_sensor_, temperature);
  this->publish_state_(this->fan_running_binary_sensor_, (bool) (temperature >= 45.0));
}

void SoyosourceDisplay::on_ms51_settings_data_(const std::vector<uint8_t> &data) {
  auto soyosource_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  // Settings response example   0x5A 0x01 0xD3 0x02 0xD4 0x30 0x31 0x2F 0x00 0xE6 0x64 0x5A 0x00 0x06 0x37 0x5A 0x8A

  ESP_LOGI(TAG, "Settings:");

  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     1   0x5A                   Header
  // 1     1   0x01
  ESP_LOGD(TAG, "  Unknown (byte 1): %02X", data[1]);

  // 2     1   0xD3                   Operation mode (High nibble), Frame function (Low nibble)
  uint8_t operation_mode_setting = this->operation_mode_to_operation_mode_setting_(data[2] >> 4);
  ESP_LOGI(TAG, "  Operation mode setting: %02X", operation_mode_setting);
  this->current_settings_.OperationMode = operation_mode_setting;
  if (this->operation_mode_select_ != nullptr) {
    for (auto &listener : this->select_listeners_) {
      if (listener.holding_register == 0x0A) {
        listener.on_value(operation_mode_setting);
      }
    }
  }

  // 3     1   0x02                   Operation status bitmask
  // 4     1   0xD4                   Device model
  ESP_LOGI(TAG, "  Device model: %d W (%d)", (data[4] % 100) * 100, data[4]);

  // 5     1   0x30                   Device type
  ESP_LOGI(TAG, "  Device type: %s (%d)", this->device_type_to_string_(data[5]).c_str(), data[5]);

  // 6     1   0x31                   Starting voltage                 V           49
  uint8_t start_voltage = data[6];
  ESP_LOGI(TAG, "  Start voltage: %d V", start_voltage);
  this->current_settings_.StartVoltage = start_voltage;
  this->publish_state_(this->start_voltage_number_, start_voltage);

  // 7     1   0x2F                   Shutdown voltage                 V           47
  uint8_t shutdown_voltage = data[7];
  ESP_LOGI(TAG, "  Shutdown voltage: %d V", shutdown_voltage);
  this->current_settings_.ShutdownVoltage = shutdown_voltage;
  this->publish_state_(this->shutdown_voltage_number_, shutdown_voltage);

  // 8     2   0x00 0xE6              Grid voltage                     V           230 V
  ESP_LOGV(TAG, "  Grid voltage: %.0f V", (float) soyosource_get_16bit(8));

  // 10    1   0x64                   Grid frequency         0.5       Hz          100 * 0.5 = 50 Hz
  uint8_t grid_frequency = data[10];
  ESP_LOGV(TAG, "  Grid frequency: %.1f Hz", (float) grid_frequency * 0.5f);
  this->current_settings_.GridFrequency = grid_frequency;

  // 11    1   0x5A                   Battery output power   10        W           90 * 10 = 900 W
  uint8_t output_power_limit = data[11];
  ESP_LOGI(TAG, "  Output power limit: %d W", output_power_limit * 10);
  this->current_settings_.OutputPowerLimit = output_power_limit;
  this->publish_state_(this->output_power_limit_number_, output_power_limit * 10);

  // 12    1   0x00
  ESP_LOGD(TAG, "  Unknown (byte 12): %02X", data[12]);

  // 13    1   0x06                   Delay in seconds                 s           6
  uint8_t start_delay = data[13];
  ESP_LOGI(TAG, "  Start delay: %d s", start_delay);
  this->current_settings_.StartDelay = start_delay;
  this->publish_state_(this->start_delay_number_, start_delay);

  // 14    1   0x37
  // 15    1   0x5A
  ESP_LOGD(TAG, "  Unknown (byte 14-15): %02X %02X", data[14], data[15]);

  // 16    1   0x8A                   Checksum
}

void SoyosourceDisplay::on_soyosource_settings_data_(const std::vector<uint8_t> &data) {
  auto soyosource_get_16bit = [&](size_t i) -> uint16_t {
    return (uint16_t(data[i + 0]) << 8) | (uint16_t(data[i + 1]) << 0);
  };

  ESP_LOGI(TAG, "Settings:");

  // Byte Len  Payload                Content              Coeff.      Unit        Example value
  // 0     1   0xA6                   Header
  // 1     1   0x00                   Unknown
  ESP_LOGD(TAG, "  Unknown (byte 1): %02X", data[1]);

  // 2     1   0x72
  ESP_LOGD(TAG, "  Unknown (byte 2): %02X", data[2]);

  // 3     1   0x93                   Operation mode (High nibble), Frame function (Low nibble)
  uint8_t operation_mode_setting = this->operation_mode_to_operation_mode_setting_(data[3] >> 4);
  ESP_LOGI(TAG, "  Operation mode setting: %02X", operation_mode_setting);
  this->current_settings_.OperationMode = operation_mode_setting;
  if (this->operation_mode_select_ != nullptr) {
    for (auto &listener : this->select_listeners_) {
      if (listener.holding_register == 0x0A) {
        listener.on_value(operation_mode_setting);
      }
    }
  }

  // 4     1   0x40                   Operation status bitmask

  // 5     1   0xD4                   Device model
  ESP_LOGI(TAG, "  Device model: %d W (%d)", (data[5] % 100) * 100, data[5]);

  // 6     1   0x30                   Device type
  ESP_LOGI(TAG, "  Device type: %s (%d)", this->device_type_to_string_(data[6]).c_str(), data[6]);

  // 7     1   0x2C                   Starting voltage                 V           44
  uint8_t start_voltage = data[7];
  ESP_LOGI(TAG, "  Start voltage: %d V", start_voltage);
  this->current_settings_.StartVoltage = start_voltage;
  this->publish_state_(this->start_voltage_number_, start_voltage);

  // 8     1   0x2B                   Shutdown voltage                 V           43
  uint8_t shutdown_voltage = data[8];
  ESP_LOGI(TAG, "  Shutdown voltage: %d V", shutdown_voltage);
  this->current_settings_.ShutdownVoltage = shutdown_voltage;
  this->publish_state_(this->shutdown_voltage_number_, shutdown_voltage);

  // 9     2   0x00 0xFA              Grid voltage                     V           250
  ESP_LOGV(TAG, "  Grid voltage: %.0f V", (float) soyosource_get_16bit(9));

  // 11     1   0x64                   Grid frequency        0.5       Hz          100
  uint8_t grid_frequency = data[11];
  ESP_LOGV(TAG, "  Grid frequency: %.1f Hz", (float) grid_frequency * 0.5f);
  this->current_settings_.GridFrequency = grid_frequency;

  // 12    1   0x5A                   Battery output power   10        W           90
  uint8_t output_power_limit = data[12];
  ESP_LOGI(TAG, "  Output power limit: %d W", output_power_limit * 10);
  this->current_settings_.OutputPowerLimit = output_power_limit;
  this->publish_state_(this->output_power_limit_number_, output_power_limit * 10);

  // 13    1   0x03                   Delay in seconds                 s           3
  uint8_t start_delay = data[13];
  ESP_LOGI(TAG, "  Start delay: %d s", start_delay);
  this->current_settings_.StartDelay = start_delay;
  this->publish_state_(this->start_delay_number_, start_delay);
}

void SoyosourceDisplay::dump_config() {
  ESP_LOGCONFIG(TAG, "SoyosourceDisplay:");
  LOG_SENSOR("", "Error bitmask", this->error_bitmask_sensor_);
  LOG_TEXT_SENSOR("", "Errors", this->errors_text_sensor_);
  LOG_SENSOR("", "Operation Mode ID", this->operation_mode_id_sensor_);
  LOG_TEXT_SENSOR("", "Operation Mode", this->operation_mode_text_sensor_);
  LOG_TEXT_SENSOR("", "Operation Status ID", this->operation_status_id_sensor_);
  LOG_TEXT_SENSOR("", "Operation Status", this->operation_status_text_sensor_);
  LOG_SENSOR("", "Battery Voltage", this->battery_voltage_sensor_);
  LOG_SENSOR("", "Battery Current", this->battery_current_sensor_);
  LOG_SENSOR("", "Battery Power", this->battery_power_sensor_);
  LOG_SENSOR("", "AC Voltage", this->ac_voltage_sensor_);
  LOG_SENSOR("", "AC Frequency", this->ac_frequency_sensor_);
  LOG_SENSOR("", "Temperature", this->temperature_sensor_);
  LOG_BINARY_SENSOR("", "Fan Running", this->fan_running_binary_sensor_);
  LOG_BINARY_SENSOR("", "Limiter Connected", this->limiter_connected_binary_sensor_);
}
float SoyosourceDisplay::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void SoyosourceDisplay::update() { this->send_command(STATUS_COMMAND); }

void SoyosourceDisplay::send_command(uint8_t function, uint8_t start_voltage, uint8_t shutdown_voltage,
                                     uint8_t output_power_limit, uint8_t grid_frequency, uint8_t start_delay,
                                     uint8_t operation_mode) {
  uint8_t frame[12];

  frame[0] = SOF_REQUEST;         // Header
  frame[1] = function;            // Function         (0x01: Status, 0x03: Settings, 0x11: Reboot, 0x0B: Write settings)
  frame[2] = start_voltage;       // Start voltage    (0x30: 48 V)
  frame[3] = shutdown_voltage;    // Shutdown voltage     (0x2D: 45 V)
  frame[4] = output_power_limit;  // Constant Power / Power Limit   (0x5A: 90 * 100 = 900 W)
  frame[5] = grid_frequency;      // Grid frequency   (0x64: 100 * 0.5 = 50 Hz)
  frame[6] = 0x00;
  frame[7] = 0x00;
  frame[8] = 0x00;
  frame[9] = start_delay;      // Start delay in seconds (0x06: 6 seconds)
  frame[10] = operation_mode;  // Operation mode (0x01: PV, 0x11: PV Limit, 0x02: Bat Const, 0x12: Bat Limit)
  frame[11] = chksum(frame, 11);

  this->write_array(frame, 12);
  this->flush();
}

void SoyosourceDisplay::update_setting(uint8_t holding_register, float value) {
  SoyosourceSettingsFrameT new_settings = this->current_settings_;

  switch (holding_register) {
    case 0x02:
      new_settings.StartVoltage = (uint8_t) value;
      break;
    case 0x03:
      new_settings.ShutdownVoltage = (uint8_t) value;
      break;
    case 0x04:
      new_settings.OutputPowerLimit = (uint8_t)(value * 0.1);
      break;
    case 0x09:
      new_settings.StartDelay = (uint8_t) value;
      break;
    case 0x0A:
      new_settings.OperationMode = (uint8_t) value;
      break;
    default:
      ESP_LOGE(TAG, "Unknown settings register. Aborting write settings");
      return;
  }

  if (new_settings.OperationMode == 0x00) {
    ESP_LOGE(TAG, "Cannot update settings because the current settings are unknown");
    return;
  }

  this->write_settings_(&new_settings);
}

void SoyosourceDisplay::write_settings_(SoyosourceSettingsFrameT *settings_frame) {
  settings_frame->Header = SOF_REQUEST;
  settings_frame->Function = WRITE_SETTINGS_COMMAND;
  settings_frame->Checksum = chksum((const uint8_t *) settings_frame, 11);

  this->write_array((const uint8_t *) settings_frame, 12);
  this->flush();
}

std::string SoyosourceDisplay::operation_mode_to_string_(const uint8_t &operation_mode) {
  // 0x01: 0001   Battery
  // 0x05: 0101   Battery + Standby
  //
  // 0x02: 0010   PV
  // 0x06: 0110   PV + Standby
  //
  // 0x09: 1001   Battery + Limiter
  // 0x0D: 1101   Battery + Limiter + Standby
  //
  // 0x0A: 1010   PV + Limiter
  // 0x0E: 1110   PV + Limiter + Standby
  //       ||||
  //       |||Battery mode bit
  //       |||
  //       ||PV mode bit
  //       ||
  //       |Standby bit
  //       |
  //       Limiter bit
  //
  switch (operation_mode) {
    case 0x01:
    case 0x05:
      return "Battery Constant Power";
    case 0x02:
    case 0x06:
      return "PV";
    case 0x09:
    case 0x0D:
      return "Battery Limit";
    case 0x0A:
    case 0x0E:
      return "PV Limit";
  }

  ESP_LOGW(TAG, "  Operation mode: Unknown (%d)", operation_mode);
  return "Unknown";
}

uint8_t SoyosourceDisplay::operation_mode_to_operation_mode_setting_(const uint8_t &operation_mode) {
  switch (operation_mode) {
    case 0x01:
    case 0x05:
      return 0x02;
    case 0x02:
    case 0x06:
      return 0x01;
    case 0x09:
    case 0x0D:
      return 0x12;
    case 0x0A:
    case 0x0E:
      return 0x11;
  }

  return 0x00;
}

std::string SoyosourceDisplay::device_type_to_string_(const uint8_t &device_type) {
  switch (device_type) {
    case 0x18:
      return "PV 26-56V / BAT 24V";
    case 0x24:
      return "PV 39-62V / BAT 36V";
    case 0x30:
      return "PV 55-90V / BAT 48V";
    case 0x48:
      return "PV 85-130V / BAT 72V";
    case 0x60:
      return "PV 120-180V / BAT 96V";
  }

  ESP_LOGW(TAG, "  Device type: Unknown (%d)", device_type);
  return "Unknown";
}

std::string SoyosourceDisplay::error_bits_to_string_(const uint8_t &error_bits) {
  bool first = true;
  std::string errors_list = "";

  if (error_bits) {
    for (int i = 0; i < ERRORS_SIZE; i++) {
      if (error_bits & (1 << i)) {
        if (first) {
          first = false;
        } else {
          errors_list.append(";");
        }
        errors_list.append(ERRORS[i]);
      }
    }
  }

  return errors_list;
}

void SoyosourceDisplay::register_select_listener(uint8_t holding_register, const std::function<void(uint8_t)> &func) {
  auto select_listener = SoyosourceSelectListener{
      .holding_register = holding_register,
      .on_value = func,
  };
  this->select_listeners_.push_back(select_listener);
}

void SoyosourceDisplay::publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state) {
  if (binary_sensor == nullptr)
    return;

  binary_sensor->publish_state(state);
}

void SoyosourceDisplay::publish_state_(number::Number *number, float value) {
  if (number == nullptr)
    return;

  number->publish_state(value);
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

}  // namespace soyosource_display
}  // namespace esphome
