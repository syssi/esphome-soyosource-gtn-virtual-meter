#pragma once

#include "../soyosource_virtual_meter.h"
#include "esphome/core/component.h"
#include "esphome/components/switch/switch.h"

namespace esphome {
namespace soyosource_virtual_meter {

enum SoyosourceSwitchRestoreMode {
  SOYOSOURCE_SWITCH_RESTORE_DEFAULT_OFF,
  SOYOSOURCE_SWITCH_RESTORE_DEFAULT_ON,
  SOYOSOURCE_SWITCH_ALWAYS_OFF,
  SOYOSOURCE_SWITCH_ALWAYS_ON,
};

class SoyosourceVirtualMeter;

class SoyosourceSwitch : public switch_::Switch, public Component {
 public:
  void set_parent(SoyosourceVirtualMeter *parent) { this->parent_ = parent; };
  void set_restore_mode(SoyosourceSwitchRestoreMode restore_mode);
  void dump_config() override;

 protected:
  void write_state(bool state) override;
  SoyosourceVirtualMeter *parent_;
  SoyosourceSwitchRestoreMode restore_mode_{SOYOSOURCE_SWITCH_RESTORE_DEFAULT_OFF};
};

}  // namespace soyosource_virtual_meter
}  // namespace esphome
