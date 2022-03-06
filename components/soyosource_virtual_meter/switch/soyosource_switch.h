#pragma once

#include "../soyosource_virtual_meter.h"
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace soyosource_virtual_meter {

class SoyosourceVirtualMeter;

class SoyosourceSwitch : public switch_::Switch, public Component {
 public:
  void set_parent(SoyosourceVirtualMeter *parent) { this->parent_ = parent; };
  void dump_config() override;
  void loop() override {}
  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  void write_state(bool state) override;
  SoyosourceVirtualMeter *parent_;
};

}  // namespace soyosource_virtual_meter
}  // namespace esphome
