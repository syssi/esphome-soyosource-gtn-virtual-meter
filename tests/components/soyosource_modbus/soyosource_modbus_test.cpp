#include <gtest/gtest.h>
#include "common.h"

namespace esphome::soyosource_modbus::testing {

// Frame parsing and device dispatch

TEST(SoyosourceModbusParseTest, ValidFrameDispatchedToDevice) {
  TestableSoyosourceModbus modbus;
  MockSoyosourceModbusDevice device;
  device.set_address(0x23);
  modbus.register_device(&device);

  modbus.feed(MODBUS_FRAME);

  EXPECT_EQ(device.call_count, 1);
  EXPECT_EQ(device.received_data, MODBUS_FRAME_DATA);
}

TEST(SoyosourceModbusParseTest, TwoFramesDispatchedTwice) {
  TestableSoyosourceModbus modbus;
  MockSoyosourceModbusDevice device;
  device.set_address(0x23);
  modbus.register_device(&device);

  modbus.feed(MODBUS_FRAME);
  modbus.feed(MODBUS_FRAME);

  EXPECT_EQ(device.call_count, 2);
}

TEST(SoyosourceModbusParseTest, InvalidHeaderRejected) {
  TestableSoyosourceModbus modbus;
  MockSoyosourceModbusDevice device;
  device.set_address(0x23);
  modbus.register_device(&device);

  // Second header byte must be 0x01 -- replace with 0xFF to trigger reject
  std::vector<uint8_t> bad_frame = MODBUS_FRAME;
  bad_frame[1] = 0xFF;
  modbus.feed(bad_frame);

  EXPECT_EQ(device.call_count, 0);
}

TEST(SoyosourceModbusParseTest, UnknownAddressLogsWarning) {
  TestableSoyosourceModbus modbus;
  MockSoyosourceModbusDevice device;
  device.set_address(0x99);  // different address
  modbus.register_device(&device);

  modbus.feed(MODBUS_FRAME);  // address in frame is 0x23

  EXPECT_EQ(device.call_count, 0);
}

TEST(SoyosourceModbusParseTest, FrameWithoutRegisteredDeviceDoesNotCrash) {
  TestableSoyosourceModbus modbus;
  modbus.feed(MODBUS_FRAME);
}

TEST(SoyosourceModbusParseTest, PartialFrameDoesNotDispatch) {
  TestableSoyosourceModbus modbus;
  MockSoyosourceModbusDevice device;
  device.set_address(0x23);
  modbus.register_device(&device);

  // Feed only the first 10 bytes of a 15-byte frame
  for (size_t i = 0; i < 10; i++)
    modbus.parse_soyosource_modbus_byte_(MODBUS_FRAME[i]);

  EXPECT_EQ(device.call_count, 0);
}

TEST(SoyosourceModbusParseTest, DataBytesMatchFrameSlice) {
  TestableSoyosourceModbus modbus;
  MockSoyosourceModbusDevice device;
  device.set_address(0x23);
  modbus.register_device(&device);

  modbus.feed(MODBUS_FRAME);

  ASSERT_EQ(device.received_data.size(), 10u);
  for (size_t i = 0; i < 10; i++)
    EXPECT_EQ(device.received_data[i], MODBUS_FRAME[4 + i]) << "Mismatch at index " << i;
}

}  // namespace esphome::soyosource_modbus::testing
