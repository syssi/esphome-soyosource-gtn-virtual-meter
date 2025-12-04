#pragma once

#include "esphome/core/component.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/number/number.h"
#include "esphome/components/select/select.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace soyosource_display {

enum ProtocolVersion {
  SOYOSOURCE_WIFI_VERSION,
  SOYOSOURCE_DISPLAY_VERSION,
};

struct SoyosourceSettingsFrameT {
  uint8_t Header;
  uint8_t Function;
  uint8_t StartVoltage;
  uint8_t ShutdownVoltage;
  uint8_t OutputPowerLimit;
  uint8_t GridFrequency;
  uint8_t Unused[3];
  uint8_t StartDelay;
  uint8_t OperationMode;
  uint8_t Checksum;
};

struct SoyosourceSelectListener {
  uint8_t holding_register;
  std::function<void(uint8_t)> on_value;
};

class SoyosourceDisplay : public uart::UARTDevice, public PollingComponent {
 public:
  void set_fan_running_binary_sensor(binary_sensor::BinarySensor *fan_running_binary_sensor) {
    fan_running_binary_sensor_ = fan_running_binary_sensor;
  }
  void set_limiter_connected_binary_sensor(binary_sensor::BinarySensor *limiter_connected_binary_sensor) {
    limiter_connected_binary_sensor_ = limiter_connected_binary_sensor;
  }

  void set_start_voltage_number(number::Number *start_voltage_number) { start_voltage_number_ = start_voltage_number; }
  void set_shutdown_voltage_number(number::Number *shutdown_voltage_number) {
    shutdown_voltage_number_ = shutdown_voltage_number;
  }
  void set_output_power_limit_number(number::Number *output_power_limit_number) {
    output_power_limit_number_ = output_power_limit_number;
  }
  void set_start_delay_number(number::Number *start_delay_number) { start_delay_number_ = start_delay_number; }

  void set_operation_mode_select(select::Select *operation_mode_select) {
    operation_mode_select_ = operation_mode_select;
  }

  void set_error_bitmask_sensor(sensor::Sensor *error_bitmask_sensor) { error_bitmask_sensor_ = error_bitmask_sensor; }
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
  void set_total_energy_sensor(sensor::Sensor *total_energy_sensor) { total_energy_sensor_ = total_energy_sensor; }
  void set_output_power_sensor(sensor::Sensor *output_power_sensor) { output_power_sensor_ = output_power_sensor; }

  void set_errors_text_sensor(text_sensor::TextSensor *errors_text_sensor) { errors_text_sensor_ = errors_text_sensor; }
  void set_operation_mode_text_sensor(text_sensor::TextSensor *operation_mode_text_sensor) {
    operation_mode_text_sensor_ = operation_mode_text_sensor;
  }
  void set_operation_status_text_sensor(text_sensor::TextSensor *operation_status_text_sensor) {
    operation_status_text_sensor_ = operation_status_text_sensor;
  }

  void set_protocol_version(ProtocolVersion protocol_version) { protocol_version_ = protocol_version; }

  SoyosourceSettingsFrameT get_current_settings() { return current_settings_; }
  void register_select_listener(uint8_t holding_register, const std::function<void(uint8_t)> &func);
  void send_command(uint8_t function);
  void display_version_send_command(uint8_t function, uint8_t value1, uint8_t value2, uint8_t value3);
  void update_setting(uint8_t holding_register, float value);
  void loop() override;
  void dump_config() override;
  void update() override;
  float get_setup_priority() const override;

 protected:
  ProtocolVersion protocol_version_{SOYOSOURCE_WIFI_VERSION};

  binary_sensor::BinarySensor *fan_running_binary_sensor_;
  binary_sensor::BinarySensor *limiter_connected_binary_sensor_;

  number::Number *start_voltage_number_;
  number::Number *shutdown_voltage_number_;
  number::Number *output_power_limit_number_;
  number::Number *start_delay_number_;

  select::Select *operation_mode_select_;

  sensor::Sensor *error_bitmask_sensor_;
  sensor::Sensor *operation_mode_id_sensor_;
  sensor::Sensor *operation_status_id_sensor_;
  sensor::Sensor *battery_voltage_sensor_;
  sensor::Sensor *battery_current_sensor_;
  sensor::Sensor *battery_power_sensor_;
  sensor::Sensor *ac_voltage_sensor_;
  sensor::Sensor *ac_frequency_sensor_;
  sensor::Sensor *temperature_sensor_;
  sensor::Sensor *total_energy_sensor_;
  sensor::Sensor *output_power_sensor_;

  text_sensor::TextSensor *operation_mode_text_sensor_;
  text_sensor::TextSensor *operation_status_text_sensor_;
  text_sensor::TextSensor *errors_text_sensor_;

  std::vector<SoyosourceSelectListener> select_listeners_;
  std::vector<uint8_t> rx_buffer_;
  uint32_t last_byte_{0};
  SoyosourceSettingsFrameT current_settings_;

  void on_soyosource_display_data_(const uint8_t &function, const std::vector<uint8_t> &data);
  void on_soyosource_status_data_(const std::vector<uint8_t> &data);
  void on_soyosource_settings_data_(const std::vector<uint8_t> &data);
  void on_ms51_status_data_(const std::vector<uint8_t> &data);
  void on_ms51_settings_data_(const std::vector<uint8_t> &data);
  void on_ms51_v2_settings_data_(const std::vector<uint8_t> &data);
  bool parse_soyosource_display_byte_(uint8_t byte);
  void publish_state_(binary_sensor::BinarySensor *binary_sensor, const bool &state);
  void publish_state_(number::Number *number, float value);
  void publish_state_(select::Select *select, const std::string &state);
  void publish_state_(sensor::Sensor *sensor, float value);
  void publish_state_(text_sensor::TextSensor *text_sensor, const std::string &state);
  void write_settings_(SoyosourceSettingsFrameT *frame);
  void display_version_write_settings_(const uint8_t &holding_register, SoyosourceSettingsFrameT *new_settings);

  uint8_t operation_mode_to_operation_mode_setting_(const uint8_t &operation_mode);

  std::string wifi_version_operation_mode_to_string_(const uint8_t &operation_mode);
  std::string display_version_operation_mode_to_string_(const uint8_t &operation_mode);
  std::string operation_status_to_string_(const uint8_t &operation_status);
  std::string device_type_to_string_(const uint8_t &device_type);
  std::string error_bits_to_string_(const uint8_t &mask);
};

}  // namespace soyosource_display
}  // namespace esphome
