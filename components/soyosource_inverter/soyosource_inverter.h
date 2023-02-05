#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/soyosource_modbus/soyosource_modbus.h"

namespace esphome {
namespace soyosource_inverter {

static const uint8_t NO_RESPONSE_THRESHOLD = 15;

class SoyosourceInverter : public PollingComponent, public soyosource_modbus::SoyosourceModbusDevice {
 public:
  void set_fan_running_binary_sensor(binary_sensor::BinarySensor *fan_running_binary_sensor) {
    fan_running_binary_sensor_ = fan_running_binary_sensor;
  }

  void set_operation_status_id_sensor(sensor::Sensor *operation_status_id_sensor) {
    operation_status_id_sensor_ = operation_status_id_sensor;
  }
  void set_battery_voltage_sensor(sensor::Sensor *battery_voltage_sensor) {
    battery_voltage_sensor_ = battery_voltage_sensor;
  }
  void set_battery_current_sensor(sensor::Sensor *battery_current_sensor) {
    battery_current_sensor_ = battery_current_sensor;
  }
  void set_battery_power_sensor(sensor::Sensor *battery_power_sensor) { battery_power_sensor_ = battery_power_sensor; }
  void set_ac_voltage_sensor(sensor::Sensor *ac_voltage_sensor) { ac_voltage_sensor_ = ac_voltage_sensor; }
  void set_ac_frequency_sensor(sensor::Sensor *ac_frequency_sensor) { ac_frequency_sensor_ = ac_frequency_sensor; }
  void set_temperature_sensor(sensor::Sensor *temperature_sensor) { temperature_sensor_ = temperature_sensor; }

  void set_operation_status_text_sensor(text_sensor::TextSensor *operation_status_text_sensor) {
    operation_status_text_sensor_ = operation_status_text_sensor;
  }

  void update() override;

  void on_soyosource_modbus_data(const std::vector<uint8_t> &data) override;

  void dump_config() override;

 protected:
  binary_sensor::BinarySensor *fan_running_binary_sensor_;

  sensor::Sensor *operation_status_id_sensor_;
  sensor::Sensor *battery_voltage_sensor_;
  sensor::Sensor *battery_current_sensor_;
  sensor::Sensor *battery_power_sensor_;
  sensor::Sensor *ac_voltage_sensor_;
  sensor::Sensor *ac_frequency_sensor_;
  sensor::Sensor *temperature_sensor_;

  text_sensor::TextSensor *operation_status_text_sensor_;

  uint8_t no_response_count_ = 0;

  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  void publish_device_offline_();
};

}  // namespace soyosource_inverter
}  // namespace esphome
