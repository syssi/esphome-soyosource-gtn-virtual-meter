#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/soyosource_inverter/soyosource_inverter.h"

#include "rs485_status_error.h"
#include "rs485_status_normal.h"
#include "rs485_status_startup.h"

namespace esphome::soyosource_inverter::testing {

class TestableSoyosourceInverter : public SoyosourceInverter {
 public:
  void update() override {}
};

}  // namespace esphome::soyosource_inverter::testing
