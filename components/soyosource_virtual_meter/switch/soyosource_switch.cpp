#include "soyosource_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

namespace esphome {
namespace soyosource_virtual_meter {

static const char *const TAG = "soyosource_virtual_meter.switch";

void SoyosourceSwitch::dump_config() { LOG_SWITCH("", "SoyosourceVirtualMeter Switch", this); }
void SoyosourceSwitch::write_state(bool state) {
  // this->parent_->write_register(this->holding_register_, (uint16_t) state);
}

}  // namespace soyosource_virtual_meter
}  // namespace esphome
