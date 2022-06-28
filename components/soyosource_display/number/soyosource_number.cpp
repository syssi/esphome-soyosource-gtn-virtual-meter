#include "soyosource_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace soyosource_display {

static const char *const TAG = "soyosource_display.number";

void SoyosourceNumber::dump_config() { LOG_NUMBER("", "SoyosourceDisplay Number", this); }
void SoyosourceNumber::control(float value) { this->parent_->update_setting(this->holding_register_, value); }

}  // namespace soyosource_display
}  // namespace esphome
