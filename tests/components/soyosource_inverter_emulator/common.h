#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/soyosource_inverter_emulator/soyosource_inverter_emulator.h"

namespace esphome::soyosource_inverter_emulator::testing {

class TestableSoyosourceInverterEmulator : public SoyosourceInverterEmulator {
 public:
  void loop() override {}
  uint8_t last_function{0xFF};
  std::vector<uint8_t> last_data;

  using SoyosourceInverterEmulator::parse_soyosource_inverter_emulator_byte_;

  bool feed(const std::vector<uint8_t> &frame) {
    bool result = false;
    for (uint8_t byte : frame) {
      result = parse_soyosource_inverter_emulator_byte_(byte);
      if (!result)
        rx_buffer_.clear();
    }
    return result;
  }

 protected:
  void on_soyosource_inverter_emulator_data(const uint8_t &function, const std::vector<uint8_t> &data) override {
    last_function = function;
    last_data = data;
  }
};

// chksum: 0xFF minus each byte from index 1 to len-1
inline uint8_t test_chksum(const std::vector<uint8_t> &frame) {
  uint8_t crc = 0xFF;
  for (size_t i = 1; i < frame.size(); i++)
    crc -= frame[i];
  return crc;
}

// Build a complete WiFi-version request frame (12 bytes) for the given function code.
inline std::vector<uint8_t> wifi_frame(uint8_t function) {
  std::vector<uint8_t> f = {0x55, function, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  f[11] = test_chksum({f.begin(), f.begin() + 11});
  return f;
}

// Build a complete display-version request frame (6 bytes) for the given function code.
inline std::vector<uint8_t> display_frame(uint8_t function) {
  std::vector<uint8_t> f = {0x55, function, 0x00, 0x00, 0x00, 0x00};
  f[5] = test_chksum({f.begin(), f.begin() + 5});
  return f;
}

}  // namespace esphome::soyosource_inverter_emulator::testing
