#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace soyosource_display {

class SoyosourceDisplay : public uart::UARTDevice, public PollingComponent {
 public:
  void set_fan_running_binary_sensor(binary_sensor::BinarySensor *fan_running_binary_sensor) {
    fan_running_binary_sensor_ = fan_running_binary_sensor;
  }

  void set_operation_mode_id_sensor(sensor::Sensor *operation_mode_id_sensor) {
    operation_mode_id_sensor_ = operation_mode_id_sensor;
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

  void set_operation_mode_text_sensor(text_sensor::TextSensor *operation_mode_text_sensor) {
    operation_mode_text_sensor_ = operation_mode_text_sensor;
  }
  void set_operation_status_text_sensor(text_sensor::TextSensor *operation_status_text_sensor) {
    operation_status_text_sensor_ = operation_status_text_sensor;
  }

  void loop() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;

 protected:
  binary_sensor::BinarySensor *fan_running_binary_sensor_;

  sensor::Sensor *operation_mode_id_sensor_;
  sensor::Sensor *operation_status_id_sensor_;
  sensor::Sensor *battery_voltage_sensor_;
  sensor::Sensor *battery_current_sensor_;
  sensor::Sensor *battery_power_sensor_;
  sensor::Sensor *ac_voltage_sensor_;
  sensor::Sensor *ac_frequency_sensor_;
  sensor::Sensor *temperature_sensor_;

  text_sensor::TextSensor *operation_mode_text_sensor_;
  text_sensor::TextSensor *operation_status_text_sensor_;

  std::vector<uint8_t> rx_buffer_;
  uint32_t last_byte_{0};
  uint32_t last_send_{0};

  void on_soyosource_display_data_(const uint8_t &function, const std::vector<uint8_t> &data);
  void on_status_data_(const std::vector<uint8_t> &data);
  void on_settings_data_(const std::vector<uint8_t> &data);
  bool parse_soyosource_display_byte_(uint8_t byte);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  void send_command_(uint8_t function);

  std::string SoyosourceDisplay::operation_mode_to_string_(uint8_t operation_mode);
  std::string SoyosourceDisplay::operation_status_to_string_(uint8_t operation_status);
};

}  // namespace soyosource_display
}  // namespace esphome
