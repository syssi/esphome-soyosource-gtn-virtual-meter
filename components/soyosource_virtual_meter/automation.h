#pragma once

#include "esphome/core/automation.h"
#include "esphome/core/component.h"
#include "soyosource_virtual_meter.h"

namespace esphome {
namespace soyosource_virtual_meter {

template<typename... Ts> class SetPowerDemandDividerAction : public Action<Ts...> {
 public:
  explicit SetPowerDemandDividerAction(SoyosourceVirtualMeter *a_soyosource_virtual_meter)
      : soyosource_virtual_meter_(a_soyosource_virtual_meter) {}

  TEMPLATABLE_VALUE(uint8_t, power_demand_divider)

  void play(Ts... x) override {
    this->soyosource_virtual_meter_->set_power_demand_divider(this->power_demand_divider_.value(x...));
  }

 protected:
  SoyosourceVirtualMeter *soyosource_virtual_meter_;
};

template<typename... Ts> class SetBufferAction : public Action<Ts...> {
 public:
  explicit SetBufferAction(SoyosourceVirtualMeter *a_soyosource_virtual_meter)
      : soyosource_virtual_meter_(a_soyosource_virtual_meter) {}

  TEMPLATABLE_VALUE(int16_t, buffer)

  void play(Ts... x) override { this->soyosource_virtual_meter_->set_buffer(this->buffer_.value(x...)); }

 protected:
  SoyosourceVirtualMeter *soyosource_virtual_meter_;
};

}  // namespace soyosource_virtual_meter
}  // namespace esphome
