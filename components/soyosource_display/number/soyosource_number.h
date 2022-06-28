#pragma once

#include "../soyosource_display.h"
#include "esphome/core/component.h"
#include "esphome/components/number/number.h"

namespace esphome {
namespace soyosource_display {

class SoyosourceDisplay;

class SoyosourceNumber : public number::Number, public Component {
 public:
  void set_parent(SoyosourceDisplay *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };
  void dump_config() override;

 protected:
  void control(float value) override;

  SoyosourceDisplay *parent_;
  uint8_t holding_register_;
};

}  // namespace soyosource_display
}  // namespace esphome
