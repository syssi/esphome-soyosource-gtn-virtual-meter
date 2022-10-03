#include "soyosource_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace soyosource_display {

static const char *const TAG = "soyosource_display.button";

void SoyosourceButton::dump_config() { LOG_BUTTON("", "SoyosourceDisplay Button", this); }
void SoyosourceButton::press_action() {
  this->parent_->send_command(this->holding_register_, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00);
}

}  // namespace soyosource_display
}  // namespace esphome
