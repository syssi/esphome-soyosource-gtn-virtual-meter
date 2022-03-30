#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace soyosource_modbus {

class SoyosourceModbusDevice;

class SoyosourceModbus : public uart::UARTDevice, public Component {
 public:
  SoyosourceModbus() = default;

  void setup() override;

  void loop() override;

  void dump_config() override;

  void register_device(SoyosourceModbusDevice *device) { this->devices_.push_back(device); }

  float get_setup_priority() const override;

  void send(uint8_t address, uint16_t measurement);
  void query_status();

  void set_flow_control_pin(GPIOPin *flow_control_pin) { this->flow_control_pin_ = flow_control_pin; }
  void set_name(const std::string &name) { this->name_ = name; }
  std::string get_name() { return this->name_; };

 protected:
  GPIOPin *flow_control_pin_{nullptr};
  std::string name_;

  bool parse_soyosource_modbus_byte_(uint8_t byte);

  std::vector<uint8_t> rx_buffer_;
  uint32_t last_soyosource_modbus_byte_{0};
  std::vector<SoyosourceModbusDevice *> devices_;
};

uint16_t crc16(const uint8_t *data, uint8_t len);

class SoyosourceModbusDevice {
 public:
  void set_parent(SoyosourceModbus *parent) { parent_ = parent; }
  void set_address(uint8_t address) { address_ = address; }
  virtual void on_soyosource_modbus_data(const std::vector<uint8_t> &data) = 0;

  void query_status() { this->parent_->query_status(); }
  void send(uint16_t measurement) { this->parent_->send(this->address_, measurement); }
  std::string get_modbus_name() { return this->parent_->get_name(); };

 protected:
  friend SoyosourceModbus;

  SoyosourceModbus *parent_;
  uint8_t address_;
};

}  // namespace soyosource_modbus
}  // namespace esphome
