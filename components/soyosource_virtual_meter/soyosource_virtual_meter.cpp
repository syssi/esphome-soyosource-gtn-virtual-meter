#include "soyosource_virtual_meter.h"
#include "esphome/core/log.h"

namespace esphome {
namespace soyosource_virtual_meter {

static const char *const TAG = "soyosource_virtual_meter";

void SoyosourceVirtualMeter::on_soyosource_modbus_data(const std::vector<uint8_t> &data) {
  ESP_LOGW(TAG, "'%s': Unknown message received!", this->get_modbus_name());
}

void SoyosourceVirtualMeter::setup() {
  this->power_sensor_->add_on_state_callback([this](float state) {
    if (std::isnan(state))
      return;

    this->power_demand_ = this->calculate_power_demand_((int16_t) ceilf(state), this->last_power_demand_);
    ESP_LOGD(TAG, "'%s': New calculated demand: %d / last demand: %d", this->get_modbus_name(), this->power_demand_,
             this->last_power_demand_);

    this->last_power_demand_received_ = millis();

    // If the update_interval is set to never, call the update() method on every power sensor update
    if (this->get_update_interval() == SCHEDULER_DONT_RUN) {
      this->update();
    }
  });
}

void SoyosourceVirtualMeter::dump_config() {
  ESP_LOGCONFIG(TAG, "SoyosourceVirtualMeter:");
  ESP_LOGCONFIG(TAG, "  Address: 0x%02X", this->address_);
  LOG_SENSOR("", "Power Demand", this->power_demand_sensor_);
}

void SoyosourceVirtualMeter::update() {
  uint16_t power_demand = 0;
  uint16_t power_demand_per_device = 0;
  std::string operation_mode = "Unknown";

  // Manual mode
  if (this->manual_mode_switch_ != nullptr && this->manual_mode_switch_->state) {
    operation_mode = "Manual";
    if (this->manual_power_demand_number_ != nullptr && this->manual_power_demand_number_->has_state()) {
      power_demand = (uint16_t) this->manual_power_demand_number_->state;
    }  // else default = 0
  } else {
    // Automatic mode
    if (this->inactivity_timeout_()) {
      power_demand = 0;
      operation_mode = "Inactivity timeout";
      ESP_LOGW(TAG, "'%s': No power sensor update received since %d seconds. Shutting down for safety reasons.",
               this->get_modbus_name(), this->power_sensor_inactivity_timeout_s_);
    } else {
      power_demand = (uint16_t) this->power_demand_;
      operation_mode = "Auto";
    }
  }

  // Override power demand and operation mode if the operation status of the inverter isn't "normal" (optional!)
  if (this->operation_status_sensor_ != nullptr && this->operation_status_sensor_->state != 0x0) {
    power_demand = 0;
    operation_mode = "Standby";
    ESP_LOGD(TAG, "'%s': The operation mode of the inverter isn't 0x0 (normal). Suspending the limiter",
             this->get_modbus_name());
  }

  // Override power demand on emergency power off
  if (this->emergency_power_off_switch_ != nullptr && this->emergency_power_off_switch_->state) {
    power_demand = 0;
    operation_mode = "Off";
  }

  power_demand_per_device = ceilf(power_demand / float(this->power_demand_divider_));

  ESP_LOGD(TAG, "'%s': Setting the limiter to %d watts per inverter (%d in total)", this->get_modbus_name(),
           power_demand_per_device, power_demand);
  this->send(power_demand_per_device);
  this->last_power_demand_ = power_demand;
  ESP_LOGVV(TAG, "'%s': Updating last demand to: %d", this->get_modbus_name(), this->last_power_demand_);

  this->publish_state_(power_demand_sensor_, power_demand);
  this->publish_state_(this->operation_mode_text_sensor_, operation_mode);
}

int16_t SoyosourceVirtualMeter::calculate_power_demand_(int16_t consumption, uint16_t last_power_demand) {
  if (this->power_demand_calculation_ == POWER_DEMAND_CALCULATION_NEGATIVE_MEASUREMENTS_REQUIRED) {
    return this->calculate_power_demand_negative_measurements_(consumption, last_power_demand);
  }

  if (this->power_demand_calculation_ == POWER_DEMAND_CALCULATION_RESTART_ON_CROSSING_ZERO) {
    return this->calculate_power_demand_restart_on_crossing_zero_(consumption, last_power_demand);
  }

  return this->calculate_power_demand_oem_(consumption);
}

int16_t SoyosourceVirtualMeter::calculate_power_demand_negative_measurements_(int16_t consumption,
                                                                              uint16_t last_power_demand) {
  ESP_LOGD(TAG, "'%s': Using the new method to calculate the power demand: %d %d", this->get_modbus_name(), consumption,
           last_power_demand);

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
  int16_t power_demand;

  if (importing_now == 0) {
    // let's just keep it 0 then :D
    power_demand = last_power_demand;
    this->power_demand_delta_ = 0;

  } else if (this->power_demand_delta_timeout_ > 0) {
    int16_t consumption_diff = this->last_consumption_ > importing_now ? this->last_consumption_ - importing_now
                                                                       : importing_now - this->last_consumption_;
    float magic_demand_delta = abs(this->power_demand_delta_) * this->power_demand_delta_magic_constant_;
    this->last_consumption_ = importing_now;

    ESP_LOGD(TAG, "'%s': consumption_diff: %d, power_demand_delta_: %d, magic_demand_delta_: %f",
             this->get_modbus_name(), consumption_diff, this->power_demand_delta_, magic_demand_delta);

    if (this->power_demand_delta_ > 0 &&
        (consumption_diff > magic_demand_delta ||
         millis() > this->power_demand_delta_timestamp_ + this->power_demand_delta_timeout_)) {
      this->power_demand_delta_ = 0;
      ESP_LOGD(TAG, "'%s': power_demand_delta_ reset to 0 due to %s", this->get_modbus_name(),
               consumption_diff > magic_demand_delta ? "consumption_diff" : "timeout");
    }

    power_demand = importing_now + last_power_demand - this->power_demand_delta_;

    if (this->power_demand_delta_ > 0 && ((importing_now > 0 && power_demand < last_power_demand) ||
                                          (importing_now < 0 && power_demand > last_power_demand))) {
      power_demand = last_power_demand;
      ESP_LOGD(TAG, "'%s': Oscillation prevention, keeping previous demand: %d; consumption: %d",
               this->get_modbus_name(), last_power_demand, importing_now);
    }
  } else {
    power_demand = importing_now + last_power_demand;
  }

  if (power_demand >= this->max_power_demand_) {
    power_demand = this->max_power_demand_;
  }
  else if (power_demand < this->min_power_demand_) {
    power_demand = (this->zero_output_on_min_power_demand_) ? 0 : this->min_power_demand_;
  }

  if (this->power_demand_delta_ == 0 && this->power_demand_delta_timeout_ > 0) {
    this->power_demand_delta_ = power_demand - last_power_demand;
    this->power_demand_delta_timestamp_ = millis();
  }

  return power_demand;
}

int16_t SoyosourceVirtualMeter::calculate_power_demand_restart_on_crossing_zero_(int16_t consumption,
                                                                                 uint16_t last_power_demand) {
  ESP_LOGD(TAG, "'%s': Using the restart on crossing zero method to calculate the power demand: %d %d",
           this->get_modbus_name(), consumption, last_power_demand);
  if (this->buffer_ <= 0) {
    ESP_LOGE(TAG,
             "A non-positive buffer value (%d) doesn't make sense if you are using the restart on crossing zero method",
             this->buffer_);
  }

  // importing_now   consumption   buffer   last_power_demand   power_demand   return
  //     1000           1010         10          500               1500         900
  //      400            410         10          500                900         900
  //      300            310         10          500                800         800
  //       10             20         10          500                510         510
  //        5             15         10          500                505         505
  //        0             10         10          500                500         500
  //      -10              0         10          500                490           0
  //     -200           -190         10          500                300           0
  //     -500           -490         10          500                  0           0
  //     -700           -690         10          500               -200           0
  if (consumption <= 0) {
    return 0;
  }

  return this->calculate_power_demand_negative_measurements_(consumption, last_power_demand);
}

int16_t SoyosourceVirtualMeter::calculate_power_demand_oem_(int16_t consumption) {
  ESP_LOGD(TAG, "'%s': Using the dumb OEM method to calculate the power demand: %d", this->get_modbus_name(),
           consumption);

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
    return (int16_t) ((std::abs(consumption - this->buffer_) + (consumption - this->buffer_)) / 2);

  // 90: 0
  return 0;
}

bool SoyosourceVirtualMeter::inactivity_timeout_() {
  if (this->power_sensor_inactivity_timeout_s_ == 0) {
    return false;
  }

  return millis() - this->last_power_demand_received_ > (this->power_sensor_inactivity_timeout_s_ * 1000);
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

}  // namespace soyosource_virtual_meter
}  // namespace esphome
