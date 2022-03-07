#pragma once

#include "esphome/core/component.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/soyosource_modbus/soyosource_modbus.h"

namespace esphome {
namespace soyosource_virtual_meter {

class SoyosourceVirtualMeter : public PollingComponent, public soyosource_modbus::SoyosourceModbusDevice {
 public:
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_power_demand_sensor(sensor::Sensor *power_demand_sensor) { power_demand_sensor_ = power_demand_sensor; }
  void set_buffer(int16_t buffer) { this->buffer_ = buffer; }
  void set_min_power_demand(int16_t min_power_demand) { this->min_power_demand_ = min_power_demand; }
  void set_max_power_demand(int16_t max_power_demand) { this->max_power_demand_ = max_power_demand; }

  void set_manual_power_demand_number(number::Number *manual_power_demand_number) {
    manual_power_demand_number_ = manual_power_demand_number;
  }

  void set_manual_mode_switch(switch_::Switch *manual_mode_switch) { manual_mode_switch_ = manual_mode_switch; }
  void set_emergency_power_off_switch(switch_::Switch *emergency_power_off_switch) {
    emergency_power_off_switch_ = emergency_power_off_switch;
  }

  void setup() override;

  void on_soyosource_modbus_data(const std::vector<uint8_t> &data) override;
  void dump_config() override;

  void update() override;

  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  sensor::Sensor *power_sensor_;
  sensor::Sensor *power_demand_sensor_;

  number::Number *manual_power_demand_number_;

  switch_::Switch *manual_mode_switch_;
  switch_::Switch *emergency_power_off_switch_;

  int16_t buffer_;
  int16_t min_power_demand_;
  int16_t max_power_demand_;
  int16_t power_consumption_;

  void publish_state_(sensor::Sensor *sensor, float value);
  int16_t calculate_power_demand_(int16_t consumption);
};

}  // namespace soyosource_virtual_meter
}  // namespace esphome
