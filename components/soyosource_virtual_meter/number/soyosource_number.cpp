#include "soyosource_number.h"
#include "esphome/core/log.h"

namespace esphome {
namespace soyosource_virtual_meter {

static const char *const TAG = "soyosource_virtual_meter.number";

void SoyosourceNumber::setup() {
  float value;
  if (!this->restore_value_) {
    value = this->initial_value_;
  } else {
    this->pref_ = global_preferences->make_preference<float>(this->get_object_id_hash());
    if (!this->pref_.load(&value)) {
      if (!std::isnan(this->initial_value_)) {
        value = this->initial_value_;
      } else {
        value = this->traits.get_min_value();
      }
    }
  }

  this->apply_and_publish_(value);
}

void SoyosourceNumber::control(float value) {
  this->apply_and_publish_(value);

  if (this->restore_value_)
    this->pref_.save(&value);
}

void SoyosourceNumber::apply_and_publish_(float value) {
  switch (this->address_) {
    case 0x01:
      this->parent_->set_max_power_demand((int16_t) value);
      break;
    case 0x02:
      this->parent_->set_buffer((int16_t) value);
      break;
    case 0x03:
      this->parent_->set_power_demand_divider((uint8_t) value);
      break;
    case 0x04:
      this->parent_->set_power_demand_compensation_timeout_ms((uint16_t) value);
      break;
  }

  this->publish_state(value);
}

void SoyosourceNumber::dump_config() { LOG_NUMBER("", "SoyosourceVirtualMeter Number", this); }

}  // namespace soyosource_virtual_meter
}  // namespace esphome
