#include "soyosource_button.h"

namespace esphome::soyosource_display {

void SoyosourceButton::press_action() { this->parent_->send_command(this->holding_register_); }

}  // namespace esphome::soyosource_display
