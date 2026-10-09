#include "soyosource_number.h"
#include "esphome/core/log.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::soyosource_display {

ESPHOME_LOG_TAG(TAG, "soyosource_display.number");

void SoyosourceNumber::dump_config() { LOG_NUMBER("", "SoyosourceDisplay Number", this); }
void SoyosourceNumber::control(float value) { this->parent_->update_setting(this->holding_register_, value); }

}  // namespace esphome::soyosource_display
