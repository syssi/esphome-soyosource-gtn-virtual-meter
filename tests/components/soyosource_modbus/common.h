#pragma once
#include <cstdint>
#include <vector>
#include "esphome/components/soyosource_modbus/soyosource_modbus.h"

namespace esphome::soyosource_modbus::testing {

// Frame from source comments (address=0x23, 10 data bytes, 1 CRC byte):
// 0x23 0x01 0x01 0x00 | 0x00 0x02 0x3E 0x00 0x17 0x00 0xEF 0x64 0x02 0x9E | 0xB5
static const std::vector<uint8_t> MODBUS_FRAME = {0x23, 0x01, 0x01, 0x00, 0x00, 0x02, 0x3E, 0x00,
                                                  0x17, 0x00, 0xEF, 0x64, 0x02, 0x9E, 0xB5};

static const std::vector<uint8_t> MODBUS_FRAME_DATA = {0x00, 0x02, 0x3E, 0x00, 0x17, 0x00, 0xEF, 0x64, 0x02, 0x9E};

class MockSoyosourceModbusDevice : public SoyosourceModbusDevice {
 public:
  std::vector<uint8_t> received_data;
  int call_count{0};

  void on_soyosource_modbus_data(const std::vector<uint8_t> &data) override {
    received_data = data;
    call_count++;
  }
};

class TestableSoyosourceModbus : public SoyosourceModbus {
 public:
  void loop() override {}
  using SoyosourceModbus::parse_soyosource_modbus_byte_;

  bool feed(const std::vector<uint8_t> &frame) {
    bool result = false;
    for (uint8_t byte : frame) {
      result = parse_soyosource_modbus_byte_(byte);
      if (!result)
        rx_buffer_.clear();
    }
    return result;
  }
};

}  // namespace esphome::soyosource_modbus::testing
