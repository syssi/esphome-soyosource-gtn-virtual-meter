#pragma once

#include "esphome/core/component.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/soyosource_modbus/soyosource_modbus.h"

namespace esphome {
namespace soyosource_inverter {

class SoyosourceInverter : public PollingComponent, public soyosource_modbus::SoyosourceModbusDevice {
 public:
  void set_operation_mode_id_sensor(sensor::Sensor *operation_mode_id_sensor) {
    operation_mode_id_sensor_ = operation_mode_id_sensor;
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

  void update() override;

  void on_soyosource_modbus_data(const std::vector<uint8_t> &data) override;

  void dump_config() override;

 protected:
  sensor::Sensor *operation_mode_id_sensor_;
  sensor::Sensor *battery_voltage_sensor_;
  sensor::Sensor *battery_current_sensor_;
  sensor::Sensor *battery_power_sensor_;
  sensor::Sensor *ac_voltage_sensor_;
  sensor::Sensor *ac_frequency_sensor_;
  sensor::Sensor *temperature_sensor_;
};

}  // namespace soyosource_inverter
}  // namespace esphome
