#include "soyosource_inverter_emulator.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace soyosource_inverter_emulator {

static const char *const TAG = "soyosource_inverter_emulator";

static const uint8_t STATUS_ALT_COMMAND = 0x00;
static const uint8_t STATUS_COMMAND = 0x01;
static const uint8_t SETTINGS_ALT_COMMAND = 0x02;
static const uint8_t SETTINGS_COMMAND = 0x03;
static const uint8_t REBOOT_COMMAND = 0x11;
static const uint8_t WRITE_SETTINGS_COMMAND = 0x0B;

void SoyosourceInverterEmulator::loop() {
  const uint32_t now = millis();

  if (now - this->last_byte_ > 50) {
    this->rx_buffer_.clear();
    this->last_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_soyosource_inverter_emulator_byte_(byte)) {
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

bool SoyosourceInverterEmulator::parse_soyosource_inverter_emulator_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];
  uint16_t frame_len = 12;

  // Example request
  // 0x55 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFE

  if (at == 0)
    return true;

  if (raw[0] != 0x55) {
    ESP_LOGW(TAG, "Invalid header.");

    // return false to reset buffer
    return false;
  }

  // Byte 0...11
  if (at < frame_len - 1)
    return true;

  uint8_t function = raw[1];

  uint8_t computed_crc = chksum(raw, frame_len - 1);
  uint8_t remote_crc = raw[frame_len - 1];
  if (computed_crc != remote_crc) {
    ESP_LOGW(TAG, "SoyosourceInverterEmulator CRC Check failed! %02X != %02X", computed_crc, remote_crc);
    return false;
  }

  std::vector<uint8_t> data(this->rx_buffer_.begin(), this->rx_buffer_.begin() + frame_len - 1);

  this->on_soyosource_inverter_emulator_data_(function, data);

  // return false to reset buffer
  return false;
}

void SoyosourceInverterEmulator::on_soyosource_inverter_emulator_data_(const uint8_t &function,
                                                                       const std::vector<uint8_t> &data) {
  if (data.size() != 11) {
    ESP_LOGW(TAG, "Invalid size for soyosource inverter request frame!");
    return;
  }

  switch (function) {
    // Request  0x55 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFF
    // Response 0xA6 0x00 0x00 0x91 0x00 0x02 0x14 0x00 0x00 0x00 0xFB 0x64 0x02 0x13 0xE4
    //
    // Request   0x55 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFE
    // Response  0xA6 0x00 0x00 0x91 0x00 0x02 0x12 0x00 0x00 0x00 0xFB 0x64 0x02 0x13 0xE6
    case STATUS_ALT_COMMAND:
    case STATUS_COMMAND:
      this->send_status_(
          0,                                         // Power demand
          0x09,                                      // Operation mode bitmask
                                                     //   0x01: 0001 Battery
                                                     //   0x02: 0010 PV
                                                     //   0x04: 0100 Standby
                                                     //   0x08: 1000 Limiter enabled
                                                     //     Examples
                                                     //       0x09: 1001   Battery + Limiter enabled
                                                     //       0x0D: 1101   Battery + Standby + Limiter enabled
                                                     //
          0x00,                                      // Operation status bitmask
                                                     //   0x01: 0000 0001   Reserved / Unknown
                                                     //   0x02: 0000 0010   DC voltage too low
                                                     //   0x04: 0000 0100   DC voltage too high
                                                     //   0x08: 0000 1000   AC voltage too high
                                                     //   0x10: 0001 0000   AC voltage too low
                                                     //   0x20: 0010 0000   Reserved / Unknown
                                                     //   0x40: 0100 0000   Limiter connected
                                                     //   0x80: 1000 0000   Reserved / Unknown
          535,                                       // Battery voltage (n * 0.1)
          100,                                       // Battery current
          250,                                       // AC voltage
          100,                                       // AC frequency (n * 0.5)
          ((this->status_counter_ % 80) * 10) + 200  // Temperature ((n-300)*0.1), [~0x0118 = 1°C, ~0x03C0 = 79°C]
      );

      this->status_counter_++;
      break;

    // Request  0x55 0x02 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFD
    // Response 0xA6 0x00 0x00 0x93 0x00 0xD4 0x30 0x34 0x31 0x00 0xFB 0x64 0x5A 0x06 0x44
    //
    // Request  0x55 0x03 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFC
    // Response 0xA6 0x00 0x00 0x93 0x00 0xD4 0x30 0x34 0x31 0x00 0xFB 0x64 0x5A 0x06 0x44
    case SETTINGS_ALT_COMMAND:
    case SETTINGS_COMMAND:
      this->send_settings_(0x0000,  // Unknown byte 1 and 2
                           0x09,    // Operation mode
                           0x00,    // Operation status
                           0xD4,    // Device model: (value % 100) * 100 = W
                                    //   0x00: 0W, 0x01: 100W, 0x02: 200W, ..., 0x63: 9900W
                                    //   0x64: 0W, 0x65: 100W, 0x66: 200W, ..., 0xC7: 9900W
                                    //   0xC8: 0W, 0xC9, 100W, 0xCA: 200W, ..., 0xFF: 5500W
                                    //
                           0x30,    // Device type
                                    //   0x18: PV 26-56V / BAT 24V
                                    //   0x24: PV 39-62V / BAT 36V
                                    //   0x30: PV 55-90V / BAT 48V
                                    //   0x48: PV 85-130V / BAT 72V
                                    //   0x60: PV 120-180V / BAT 96V
                                    //
                           48,      // Start voltage
                           52,      // Shutdown voltage
                           244,     // AC voltage
                           100,     // AC frequency (n/2)
                           90,      // Max Power (n*10)
                           6        // Start delay
      );
      this->settings_counter_++;
      break;
    case WRITE_SETTINGS_COMMAND:
      ESP_LOGI(TAG, "Write settings: %s", format_hex_pretty(&data.front(), data.size()).c_str());
      break;
    case REBOOT_COMMAND:
      ESP_LOGI(TAG, "Reboot command received");
      break;
    default:
      ESP_LOGW(TAG, "Unhandled request received: %s", format_hex_pretty(&data.front(), data.size()).c_str());
  }
}

void SoyosourceInverterEmulator::dump_config() { ESP_LOGCONFIG(TAG, "SoyosourceInverterEmulator:"); }
float SoyosourceInverterEmulator::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void SoyosourceInverterEmulator::send_status_(const uint16_t &power_demand, const uint8_t &operation_mode,
                                              const uint8_t &operation_status, const uint16_t &battery_voltage,
                                              const uint16_t &battery_current, const uint16_t &ac_voltage,
                                              const uint8_t &ac_frequency, const uint16_t &temperature) {
  uint8_t frame[15];

  frame[0] = 0xA6;
  frame[1] = power_demand >> 8;
  frame[2] = power_demand;
  frame[3] = (operation_mode << 4) | STATUS_COMMAND;
  frame[4] = operation_status;
  frame[5] = battery_voltage >> 8;
  frame[6] = battery_voltage;
  frame[7] = battery_current >> 8;
  frame[8] = battery_current;
  frame[9] = ac_voltage >> 8;
  frame[10] = ac_voltage;
  frame[11] = ac_frequency;
  frame[12] = temperature >> 8;
  frame[13] = temperature;
  frame[14] = chksum(frame, 14);

  this->write_array(frame, 15);
  this->flush();
}

void SoyosourceInverterEmulator::send_settings_(const uint16_t &unknown1, const uint8_t &operation_mode,
                                                const uint8_t &operation_status, const uint8_t &device_model,
                                                const uint8_t &device_type, const uint8_t &start_voltage,
                                                const uint8_t &shutdown_voltage, const uint16_t &ac_voltage,
                                                const uint8_t &ac_frequency, const uint8_t &max_power,
                                                const uint8_t &start_delay) {
  uint8_t frame[15];

  frame[0] = 0xA6;
  frame[1] = unknown1 >> 8;
  frame[2] = unknown1;
  frame[3] = (operation_mode << 4) | SETTINGS_COMMAND;
  frame[4] = operation_status;
  frame[5] = device_model;
  frame[6] = device_type;
  frame[7] = start_voltage;
  frame[8] = shutdown_voltage;
  frame[9] = ac_voltage >> 8;
  frame[10] = ac_voltage;
  frame[11] = ac_frequency;
  frame[12] = max_power;
  frame[13] = start_delay;
  frame[14] = chksum(frame, 14);

  this->write_array(frame, 15);
  this->flush();
}

}  // namespace soyosource_inverter_emulator
}  // namespace esphome
