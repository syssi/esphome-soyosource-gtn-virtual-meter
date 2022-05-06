#pragma once

#include "esphome/core/component.h"
#include "esphome/components/uart/uart.h"

namespace esphome {
namespace soyosource_inverter_emulator {

class SoyosourceInverterEmulator : public uart::UARTDevice, public Component {
 public:
  void loop() override;
  void dump_config() override;
  float get_setup_priority() const override;

 protected:
  std::vector<uint8_t> rx_buffer_;
  uint32_t last_byte_{0};
  uint16_t status_counter_{0};
  uint16_t settings_counter_{0};

  void on_soyosource_inverter_emulator_data_(const uint8_t &function, const std::vector<uint8_t> &data);
  bool parse_soyosource_inverter_emulator_byte_(uint8_t byte);
  void send_status_(const uint16_t &power_demand, const uint8_t &operation_mode, const uint8_t &operation_status,
                    const uint16_t &battery_voltage, const uint16_t &battery_current, const uint16_t &ac_voltage,
                    const uint8_t &ac_frequency, const uint16_t &temperature);
  void send_settings_(const uint16_t &unknown1, const uint8_t &operation_mode, const uint8_t &operation_status,
                      const uint8_t &device_model, const uint8_t &device_type, const uint8_t &start_voltage,
                      const uint8_t &shutdown_voltage, const uint16_t &ac_voltage, const uint8_t &ac_frequency,
                      const uint8_t &max_power, const uint8_t &start_delay);
  std::string operation_mode_to_string_(const uint8_t &operation_mode);
  std::string operation_status_to_string_(const uint8_t &operation_status);
};

}  // namespace soyosource_inverter_emulator
}  // namespace esphome
