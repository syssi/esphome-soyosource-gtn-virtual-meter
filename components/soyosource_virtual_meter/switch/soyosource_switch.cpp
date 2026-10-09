#include "soyosource_switch.h"
#include "esphome/core/log.h"
#include "esphome/core/application.h"

// Fallback for ESPHome < 2026.10.0
#ifndef ESPHOME_LOG_TAG
#define ESPHOME_LOG_TAG(name, tag) static const char *const name = tag
#endif

namespace esphome::soyosource_virtual_meter {

ESPHOME_LOG_TAG(TAG, "soyosource_virtual_meter.switch");

void SoyosourceSwitch::setup() {
  ESP_LOGCONFIG(TAG, "Setting up Soyosource Switch '%s'...", this->name_.c_str());

  bool initial_state = false;
  switch (this->restore_mode_) {
    case SOYOSOURCE_SWITCH_RESTORE_DEFAULT_OFF:
      initial_state = this->get_initial_state().value_or(false);
      break;
    case SOYOSOURCE_SWITCH_RESTORE_DEFAULT_ON:
      initial_state = this->get_initial_state().value_or(true);
      break;
    case SOYOSOURCE_SWITCH_ALWAYS_OFF:
      initial_state = false;
      break;
    case SOYOSOURCE_SWITCH_ALWAYS_ON:
      initial_state = true;
      break;
  }

  if (initial_state) {
    this->turn_on();
  } else {
    this->turn_off();
  }
}
void SoyosourceSwitch::dump_config() {
  LOG_SWITCH("", "SoyosourceVirtualMeter Switch", this);
  // NOLINTNEXTLINE(clang-analyzer-deadcode.DeadStores)
  const LogString *restore_mode = LOG_STR("Unknown");
  switch (this->restore_mode_) {
    case SOYOSOURCE_SWITCH_RESTORE_DEFAULT_OFF:
      restore_mode = LOG_STR("Restore (Defaults to OFF)");
      break;
    case SOYOSOURCE_SWITCH_RESTORE_DEFAULT_ON:
      restore_mode = LOG_STR("Restore (Defaults to ON)");
      break;
    case SOYOSOURCE_SWITCH_ALWAYS_OFF:
      restore_mode = LOG_STR("Always OFF");
      break;
    case SOYOSOURCE_SWITCH_ALWAYS_ON:
      restore_mode = LOG_STR("Always ON");
      break;
  }
  ESP_LOGCONFIG(TAG, "  Restore Mode: %s", LOG_STR_ARG(restore_mode));
}
void SoyosourceSwitch::write_state(bool state) { this->publish_state(state); }

}  // namespace esphome::soyosource_virtual_meter
