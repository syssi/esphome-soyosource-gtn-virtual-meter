#include "soyosource_virtual_meter.h"
#include "esphome/core/log.h"

namespace esphome {
namespace soyosource_virtual_meter {

static const char *const TAG = "soyosource_virtual_meter";

void SoyosourceVirtualMeter::on_soyosource_modbus_data(const std::vector<uint8_t> &data) {
  ESP_LOGW(TAG, "Unknown message received!");
}

void SoyosourceVirtualMeter::setup() {
  this->power_sensor_->add_on_state_callback([this](float state) {
    if (std::isnan(state))
      return;

    this->power_consumption_ = (int16_t) ceilf(state);
  });
}

void SoyosourceVirtualMeter::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void SoyosourceVirtualMeter::dump_config() {
  ESP_LOGCONFIG(TAG, "SoyosourceVirtualMeter:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("", "Power Demand", this->power_demand_sensor_);
}

void SoyosourceVirtualMeter::update() {
  uint16_t power_demand = 0;

  if (this->manual_mode_ && manual_power_demand_number_ != nullptr) {
    power_demand = this->manual_power_demand_;
  } else {
    power_demand = (uint16_t) this->calculate_power_demand_(this->power_consumption_);
  }

  ESP_LOGD(TAG, "Setting the limiter to %d watts", power_demand);
  this->send(power_demand);

  this->publish_state_(power_demand_sensor_, power_demand);
}

int16_t SoyosourceVirtualMeter::calculate_power_demand_(int16_t consumption) {
  // 5000 > 2000 + 10: 2000
  // 2011 > 2000 + 10: 2000
  // 2010 > 2000 + 10: continue
  // 500 > 2000 + 10: continue
  if (consumption > this->max_power_demand_ + this->buffer_)
    return this->max_power_demand_;

  // 5000 > 2000: abs(10 - 2000) = 1990 (already handled above!)
  // 2011 > 2000: abs(10 - 2000) = 1990 (already handled above!)
  // 2010 > 2000: abs(10 - 2000) = 1990
  // 2001 > 2000: abs(10 - 2000) = 1990
  // 2000 > 2000: continue
  //  500 > 2000: continue
  if (consumption > this->max_power_demand_)
    return std::abs(this->buffer_ - this->max_power_demand_);

  // 2001 >= 100: (abs(2001 - 10) + (2001 - 10)) / 2 = 1991 (already handled above!)
  // 2000 >= 100: (abs(2000 - 10) + (2000 - 10)) / 2 = 1990
  //  500 >= 100: (abs(500 - 10) + (500 - 10)) / 2 = 490
  //  100 >= 100: (abs(100 - 10) + (100 - 10)) / 2 = 90
  //   90 >= 100: continue
  if (consumption >= this->min_power_demand_)
    return (int16_t)((std::abs(consumption - this->buffer_) + (consumption - this->buffer_)) / 2);

  // 90: 0
  return 0;
}

}  // namespace soyosource_virtual_meter
}  // namespace esphome
