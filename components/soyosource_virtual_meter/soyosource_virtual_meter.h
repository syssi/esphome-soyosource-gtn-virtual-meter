#pragma once

#include "esphome/core/component.h"
#include "esphome/components/number/number.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/switch/switch.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "../soyosource_modbus/soyosource_modbus.h"

namespace esphome {
namespace soyosource_virtual_meter {

enum PowerDemandCalculation {
  POWER_DEMAND_CALCULATION_DUMB_OEM_BEHAVIOR,
  POWER_DEMAND_CALCULATION_NEGATIVE_MEASUREMENTS_REQUIRED,
  POWER_DEMAND_CALCULATION_RESTART_ON_CROSSING_ZERO,
};

class SoyosourceVirtualMeter : public PollingComponent, public soyosource_modbus::SoyosourceModbusDevice {
 public:
  void set_power_sensor(sensor::Sensor *power_sensor) { power_sensor_ = power_sensor; }
  void set_operation_status_sensor(sensor::Sensor *operation_status_sensor) {
    operation_status_sensor_ = operation_status_sensor;
  }
  void set_power_demand_sensor(sensor::Sensor *power_demand_sensor) { power_demand_sensor_ = power_demand_sensor; }
  void set_buffer(int16_t buffer) { this->buffer_ = buffer; }
  void set_min_power_demand(int16_t min_power_demand) { this->min_power_demand_ = min_power_demand; }
  void set_max_power_demand(int16_t max_power_demand) { this->max_power_demand_ = max_power_demand; }
  void set_zero_output_on_min_power_demand(bool zero_output_on_min_power_demand) {
    this->zero_output_on_min_power_demand_ = zero_output_on_min_power_demand;
  }
  void set_power_demand_divider(uint8_t power_demand_divider) { this->power_demand_divider_ = power_demand_divider; }
  void set_power_sensor_inactivity_timeout(uint16_t power_sensor_inactivity_timeout_s) {
    this->power_sensor_inactivity_timeout_s_ = power_sensor_inactivity_timeout_s;
  }
  void set_power_demand_calculation(PowerDemandCalculation power_demand_calculation) {
    this->power_demand_calculation_ = power_demand_calculation;
  }

  void set_buffer_number(number::Number *buffer_number) { buffer_number_ = buffer_number; }
  void set_manual_power_demand_number(number::Number *manual_power_demand_number) {
    manual_power_demand_number_ = manual_power_demand_number;
  }
  void set_max_power_demand_number(number::Number *max_power_demand_number) {
    max_power_demand_number_ = max_power_demand_number;
  }
  void set_power_demand_divider_number(number::Number *power_demand_divider_number) {
    power_demand_divider_number_ = power_demand_divider_number;
  }
  void set_max_power_sensor_latency_ms_number(number::Number *max_power_sensor_latency_ms_number) {
    this->power_demand_compensation_timeout_ms_number = max_power_sensor_latency_ms_number;
  }
  void set_power_demand_compensation_timeout_ms(uint16_t power_demand_compensation_timeout_ms) {
    this->power_demand_compensation_timeout_ms_ = power_demand_compensation_timeout_ms;
  }

  void set_manual_mode_switch(switch_::Switch *manual_mode_switch) { manual_mode_switch_ = manual_mode_switch; }
  void set_emergency_power_off_switch(switch_::Switch *emergency_power_off_switch) {
    emergency_power_off_switch_ = emergency_power_off_switch;
  }

  void set_operation_mode_text_sensor(text_sensor::TextSensor *operation_mode_text_sensor) {
    operation_mode_text_sensor_ = operation_mode_text_sensor;
  }

  void setup() override;

  void on_soyosource_modbus_data(const std::vector<uint8_t> &data) override;
  void dump_config() override;

  void update() override;

  float get_setup_priority() const override { return setup_priority::DATA; }

 protected:
  PowerDemandCalculation power_demand_calculation_{POWER_DEMAND_CALCULATION_DUMB_OEM_BEHAVIOR};

  number::Number *buffer_number_;
  number::Number *manual_power_demand_number_;
  number::Number *max_power_demand_number_;
  number::Number *power_demand_divider_number_;
  number::Number *power_demand_compensation_timeout_ms_number;

  sensor::Sensor *power_sensor_;
  sensor::Sensor *operation_status_sensor_;
  sensor::Sensor *power_demand_sensor_;

  switch_::Switch *manual_mode_switch_;
  switch_::Switch *emergency_power_off_switch_;

  text_sensor::TextSensor *operation_mode_text_sensor_;

  bool zero_output_on_min_power_demand_{true};
  int16_t buffer_;
  int16_t min_power_demand_;
  int16_t max_power_demand_;
  int16_t power_demand_;
  uint16_t power_sensor_inactivity_timeout_s_;
  uint8_t power_demand_divider_;

  uint32_t last_power_demand_received_{0};
  uint16_t last_power_demand_{0};

  int16_t power_demand_compensation_{0};
  uint32_t power_demand_compensation_timestamp_;
  uint16_t power_demand_compensation_timeout_ms_{5000};

  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  bool inactivity_timeout_();

  int16_t calculate_power_demand_(int16_t consumption, uint16_t last_power_demand);
  int16_t calculate_power_demand_negative_measurements_(int16_t consumption, uint16_t last_power_demand);
  int16_t calculate_power_demand_restart_on_crossing_zero_(int16_t consumption, uint16_t last_power_demand);
  int16_t calculate_power_demand_oem_(int16_t consumption);
  void reset_power_demand_compensation_(int16_t importing_now);
};

}  // namespace soyosource_virtual_meter
}  // namespace esphome
