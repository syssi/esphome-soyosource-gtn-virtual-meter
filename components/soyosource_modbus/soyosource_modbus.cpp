#include "soyosource_modbus.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace soyosource_modbus {

static const char *const TAG = "soyosource_modbus";

void SoyosourceModbus::setup() {
  if (this->flow_control_pin_ != nullptr) {
    this->flow_control_pin_->setup();
  }
}
void SoyosourceModbus::loop() {
  const uint32_t now = millis();
  if (now - this->last_soyosource_modbus_byte_ > 50) {
    this->rx_buffer_.clear();
    this->last_soyosource_modbus_byte_ = now;
  }

  while (this->available()) {
    uint8_t byte;
    this->read_byte(&byte);
    if (this->parse_soyosource_modbus_byte_(byte)) {
      this->last_soyosource_modbus_byte_ = now;
    } else {
      this->rx_buffer_.clear();
    }
  }
}

bool SoyosourceModbus::parse_soyosource_modbus_byte_(uint8_t byte) {
  size_t at = this->rx_buffer_.size();
  this->rx_buffer_.push_back(byte);
  const uint8_t *raw = &this->rx_buffer_[0];

  // Soyosource info frame: 0x23 0x01 0x01 0x00 0x00 0x02 0x3E 0x00 0x17 0x00 0xEF 0x64 0x02 0x9E 0xB5
  //                          0    1    2    3    4    5    6    7    8    9   10    11  12   13   14

  // Byte 0: soyosource_modbus address (match all)
  if (at == 0)
    return true;
  uint8_t address = raw[0];

  if (at == 3) {
    if (raw[0] != 0x23 || raw[1] != 0x01 || raw[2] != 0x01 || raw[3] != 0x00) {
      ESP_LOGW(TAG, "Invalid header: 0x%02X 0x%02X 0x%02X 0x%02X", raw[0], raw[1], raw[2], raw[3]);

      // return false to reset buffer
      return false;
    }

    return true;
  }

  // Byte 4..15: Data + CRC
  if (at < 4 + 10)
    return true;

  ESP_LOGVV(TAG, "RX <- %s", format_hex_pretty(raw, at + 1).c_str());
  ESP_LOGVV(TAG, "CRC: 0x%02X", raw[14]);

  std::vector<uint8_t> data(this->rx_buffer_.begin() + 4, this->rx_buffer_.begin() + 4 + 10);

  bool found = false;
  for (auto *device : this->devices_) {
    if (device->address_ == address) {
      device->on_soyosource_modbus_data(data);
      found = true;
    }
  }
  if (!found) {
    ESP_LOGW(TAG, "Got SoyosourceModbus frame from unknown address 0x%02X!", address);
  }

  // return false to reset buffer
  return false;
}

void SoyosourceModbus::dump_config() {
  ESP_LOGCONFIG(TAG, "SoyosourceModbus:");
  LOG_PIN("  Flow Control Pin: ", this->flow_control_pin_);

  this->check_uart_settings(4800);
}

float SoyosourceModbus::get_setup_priority() const {
  // After UART bus
  return setup_priority::BUS - 1.0f;
}

void SoyosourceModbus::send(uint8_t address, uint16_t measurement) {
  // 239V AC 1 Unit
  // 24 56 00 21 00 00 80 08 -> 0 Watt
  // 24 56 00 21 00 28 80 E0 -> 40 Watt 28h -> 40d 1*40 = 40
  //
  // 239V AC 2 Unit
  // 24 56 00 21 00 00 80 08 -> 0 Watt
  // 24 56 00 21 00 14 80 F4 -> 40 Watt 14h -> 20d 2*20 = 40
  //
  // 239V AC 3 Unit
  // 24 56 00 21 00 00 80 08 -> 0 Watt
  // 24 56 00 21 00 0D 80 FB -> 40 Watt 0dh -> 13d 3*13 = 39
  //
  // 239V AC 4 Unit
  // 24 56 00 21 00 00 80 08 -> 0 Watt
  // 24 56 00 21 00 0A 80 FE -> 40 Watt 0Ah -> 10d 4*10 = 40
  //
  // 239V AC 5 Unit
  // 24 56 00 21 00 00 80 08 -> 0 Watt
  // 24 56 00 21 00 08 80 00 -> 40 Watt 08h -> 8d 5*8 = 40

  uint8_t frame[8];
  frame[0] = address;
  frame[1] = 0x56;
  frame[2] = 0x00;
  frame[3] = 0x21;
  frame[4] = measurement >> 8;
  frame[5] = measurement >> 0;
  frame[6] = 0x80;
  frame[7] = 264 - frame[4] - frame[5];

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(true);

  this->write_array(frame, 8);
  this->flush();

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(false);
}

void SoyosourceModbus::query_status() {
  uint8_t frame[8] = {0x24, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(true);

  this->write_array(frame, 8);
  this->flush();

  if (this->flow_control_pin_ != nullptr)
    this->flow_control_pin_->digital_write(false);
}

}  // namespace soyosource_modbus
}  // namespace esphome
