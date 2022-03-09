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

    this->power_demand_ = this->calculate_power_demand_((int16_t) ceilf(state), this->last_power_demand_);
    ESP_LOGD(TAG, "New calculated demand: %d / last demand: %d", this->power_demand_, this->last_power_demand_);

    this->last_power_demand_received_ = millis();
  });
}

void SoyosourceVirtualMeter::publish_state_(sensor::Sensor *sensor, float value) {
  if (sensor == nullptr)
    return;

  sensor->publish_state(value);
}

void SoyosourceVirtualMeter::publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state) {
  if (text_sensor == nullptr)
    return;

  text_sensor->publish_state(state);
}

void SoyosourceVirtualMeter::dump_config() {
  ESP_LOGCONFIG(TAG, "SoyosourceVirtualMeter:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("", "Power Demand", this->power_demand_sensor_);
}

void SoyosourceVirtualMeter::update() {
  uint16_t power_demand = 0;
  std::string operation_mode = "Unknown";

  // Manual mode
  if (this->manual_mode_switch_ != nullptr && this->manual_mode_switch_->state) {
    operation_mode = "Manual";
    if (this->manual_power_demand_number_ != nullptr && this->manual_power_demand_number_->has_state()) {
      power_demand = (uint16_t) this->manual_power_demand_number_->state;
    }  // else default = 0
  } else {
    // Automatic mode
    if (millis() - this->last_power_demand_received_ < (this->power_sensor_inactivity_timeout_s_ * 1000)) {
      power_demand = (uint16_t) this->power_demand_;
      operation_mode = "Auto";
    } else {
      power_demand = 0;
      operation_mode = "Inactivity timeout";
      ESP_LOGW(TAG, "No power sensor update received since %d seconds. Shutting down for safety reasons.",
               this->power_sensor_inactivity_timeout_s_);
    }
  }

  // Override power demand on emergency power off
  if (this->emergency_power_off_switch_ != nullptr && this->emergency_power_off_switch_->state) {
    power_demand = 0;
    operation_mode = "Off";
  }

  ESP_LOGD(TAG, "Setting the limiter to %d watts", power_demand);
  this->send(power_demand);
  this->last_power_demand_ = power_demand;
  ESP_LOGVV(TAG, "Updating last demand to: %d", this->last_power_demand_);

  this->publish_state_(power_demand_sensor_, power_demand);
  this->publish_state_(this->operation_mode_text_sensor_, operation_mode);
}

int16_t SoyosourceVirtualMeter::calculate_power_demand_(int16_t consumption, uint16_t last_power_demand) {
  if (this->power_demand_calculation_ == POWER_DEMAND_CALCULATION_NEGATIVE_MEASUREMENTS_REQUIRED) {
    return this->calculate_power_demand_negative_measurements_(consumption, last_power_demand);
  }

  return this->calculate_power_demand_oem_(consumption);
}

int16_t SoyosourceVirtualMeter::calculate_power_demand_negative_measurements_(int16_t consumption,
                                                                              uint16_t last_power_demand) {
  ESP_LOGD(TAG, "Using the new method to calculate the power demand: %d %d", consumption, last_power_demand);

  // importing_now   consumption   buffer   last_power_demand   power_demand   return
  //     1000           1010         10          500               1500         900
  //      400            410         10          500                900         900
  //      300            310         10          500                800         800
  //       10             20         10          500                510         510
  //        0             10         10          500                500         500
  //     -200           -190         10          500                300         300
  //     -500           -490         10          500                  0           0
  //     -700           -690         10          500               -200           0
  int16_t importing_now = consumption - this->buffer_;
  int16_t power_demand = importing_now + last_power_demand;

  if (power_demand >= this->max_power_demand_) {
    return this->max_power_demand_;
  }

  if (power_demand > 0) {
    return power_demand;
  }

  return 0;
}

int16_t SoyosourceVirtualMeter::calculate_power_demand_oem_(int16_t consumption) {
  ESP_LOGD(TAG, "Using the dumb OEM method to calculate the power demand: %d", consumption);

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
