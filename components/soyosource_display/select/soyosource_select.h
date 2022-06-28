#pragma once

#include <utility>
#include <map>

#include "../soyosource_display.h"
#include "esphome/core/component.h"
#include "esphome/components/select/select.h"

namespace esphome {
namespace soyosource_display {

class SoyosourceDisplay;

class SoyosourceSelect : public select::Select, public Component {
 public:
  void set_parent(SoyosourceDisplay *parent) { this->parent_ = parent; };
  void set_holding_register(uint8_t holding_register) { this->holding_register_ = holding_register; };
  void set_select_mappings(std::vector<uint8_t> mappings) { this->mappings_ = std::move(mappings); }

  void setup() override;
  void dump_config() override;

 protected:
  void control(const std::string &value) override;

  std::vector<uint8_t> mappings_;
  SoyosourceDisplay *parent_;
  uint8_t holding_register_;
};

}  // namespace soyosource_display
}  // namespace esphome
