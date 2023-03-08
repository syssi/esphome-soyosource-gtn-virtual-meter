#pragma once

#include "../soyosource_virtual_meter.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"
#include "esphome/core/preferences.h"

namespace esphome {
namespace soyosource_virtual_meter {

class SoyosourceVirtualMeter;

class SoyosourceNumber : public number::Number, public Component {
 public:
  void set_parent(SoyosourceVirtualMeter *parent) { this->parent_ = parent; }
  void set_initial_value(float initial_value) { initial_value_ = initial_value; }
  void set_restore_value(bool restore_value) { this->restore_value_ = restore_value; }
  void set_address(uint8_t address) { this->address_ = address; };

  void setup() override;
  void dump_config() override;

 protected:
  void control(float value) override;
  void apply_and_publish_(float value);

  SoyosourceVirtualMeter *parent_;
  float initial_value_{NAN};
  bool restore_value_{false};
  uint8_t address_;

  ESPPreferenceObject pref_;
};

}  // namespace soyosource_virtual_meter
}  // namespace esphome
