#include "soyosource_button.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::soyosource_display {

ESPHOME_LOG_TAG(TAG, "soyosource_display.button");

void SoyosourceButton::dump_config() { LOG_BUTTON("", "SoyosourceDisplay Button", this); }
void SoyosourceButton::press_action() { this->parent_->send_command(this->holding_register_); }

}  // namespace esphome::soyosource_display
