#pragma once

#include "../soyosource_display.h"
#include "esphome/components/button/button.h"

namespace esphome::soyosource_display {

class SoyosourceDisplay;

class SoyosourceButton : public button::Button {
 public:
  void set_parent(SoyosourceDisplay *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };

 protected:
  void press_action() override;
  SoyosourceDisplay *parent_;
  uint8_t holding_register_;
};

}  // namespace esphome::soyosource_display
