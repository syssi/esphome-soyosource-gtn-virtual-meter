#pragma once

#include "../soyosource_virtual_meter.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace soyosource_virtual_meter {

class SoyosourceVirtualMeter;

class SoyosourceNumber : public number::Number, public Component {
 public:
  void set_parent(SoyosourceVirtualMeter *parent) { this->parent_ = parent; }
  void dump_config() override;

 protected:
  void control(float value) override;
  SoyosourceVirtualMeter *parent_;
};

}  // namespace soyosource_virtual_meter
}  // namespace esphome
