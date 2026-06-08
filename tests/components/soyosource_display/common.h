#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/number/number.h"
#include "esphome/components/soyosource_display/soyosource_display.h"

#include "ms51_settings.h"
#include "ms51_status.h"
#include "ms51_v2_settings.h"
#include "soyo_settings_display_version.h"
#include "soyo_settings_wifi_version.h"
#include "soyo_status_display_and_wifi_version.h"
#include "soyo_status_display_version.h"
#include "soyo_status_wifi_version.h"

namespace esphome::soyosource_display::testing {

class TestableNumber : public number::Number {
 protected:
  void control(float value) override { this->publish_state(value); }
};

class TestableSoyosourceDisplay : public SoyosourceDisplay {
 public:
  int send_command_calls{0};
  uint8_t last_send_command_arg{0};

  void update() override {}
  void send_command(uint8_t function) override {
    send_command_calls++;
    last_send_command_arg = function;
  }

  // Direct handler accessors bypass send_command() (which would crash with null UART parent).
  void on_soyo_status(const std::vector<uint8_t> &data) { this->on_soyosource_status_data_(data); }
  void on_soyo_settings(const std::vector<uint8_t> &data) { this->on_soyosource_settings_data_(data); }
  void on_soyo_display_data(uint8_t function, const std::vector<uint8_t> &data) {
    this->on_soyosource_display_data_(function, data);
  }
  void on_ms51_status(const std::vector<uint8_t> &data) { this->on_ms51_status_data_(data); }
  void on_ms51_settings(const std::vector<uint8_t> &data) { this->on_ms51_settings_data_(data); }
  void on_ms51_v2_settings(const std::vector<uint8_t> &data) { this->on_ms51_v2_settings_data_(data); }

  void set_protocol(ProtocolVersion v) { this->protocol_version_ = v; }
};

}  // namespace esphome::soyosource_display::testing
