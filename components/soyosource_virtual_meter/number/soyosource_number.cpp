#include "soyosource_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace soyosource_virtual_meter {

static const char *const TAG = "soyosource_virtual_meter.number";

void SoyosourceNumber::control(float value) {
  this->parent_->set_manual_power_demand((uint16_t) value);
  this->publish_state(value);
}
void SoyosourceNumber::dump_config() { LOG_NUMBER(TAG, "SoyosourceVirtualMeter Number", this); }

}  // namespace soyosource_virtual_meter
}  // namespace esphome
