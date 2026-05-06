#pragma once
#include <cstdint>
#include "esphome/components/soyosource_modbus/soyosource_modbus.h"
#include "esphome/components/soyosource_virtual_meter/soyosource_virtual_meter.h"

namespace esphome::soyosource_virtual_meter::testing {

// Minimal stub so get_modbus_name() doesn't crash (the log calls in
// calculate_power_demand_* dereference parent_->get_name()).
class StubModbus : public soyosource_modbus::SoyosourceModbus {
 public:
  StubModbus() { this->set_name("test"); }
  void setup() override {}
  void loop() override {}
};

class TestableSoyosourceVirtualMeter : public SoyosourceVirtualMeter {
 public:
  TestableSoyosourceVirtualMeter() { this->set_parent(&stub_modbus_); }
  void update() override {}
  void setup() override {}

  int16_t calc_oem(int16_t consumption) { return this->calculate_power_demand_oem_(consumption); }

  int16_t calc_neg(int16_t consumption, uint16_t last) {
    return this->calculate_power_demand_negative_measurements_(consumption, last);
  }

  int16_t calc_zero(int16_t consumption, uint16_t last) {
    return this->calculate_power_demand_restart_on_crossing_zero_(consumption, last);
  }

 private:
  StubModbus stub_modbus_;
};

}  // namespace esphome::soyosource_virtual_meter::testing
