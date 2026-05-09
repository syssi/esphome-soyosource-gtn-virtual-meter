#include <gtest/gtest.h>
#include "common.h"

namespace esphome::soyosource_inverter_emulator::testing {

// Frame construction helper

TEST(ChksumTest, StatusCommandWifi) {
  // Known valid frame from source comments:
  // 0x55 0x01 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0x00 0xFE
  auto frame = wifi_frame(0x01);
  EXPECT_EQ(frame[11], 0xFE);
}

TEST(ChksumTest, SettingsCommandWifi) {
  // 0x55 0x03 0x00 ... 0xFC
  auto frame = wifi_frame(0x03);
  EXPECT_EQ(frame[11], 0xFC);
}

TEST(ChksumTest, StatusCommandDisplay) {
  // 0x55 0x01 0x00 0x00 0x00 0xFE
  auto frame = display_frame(0x01);
  EXPECT_EQ(frame[5], 0xFE);
}

// WiFi version parsing

TEST(WifiVersionTest, InvalidHeaderRejected) {
  TestableSoyosourceInverterEmulator emu;
  emu.set_protocol_version(SOYOSOURCE_WIFI_VERSION);
  // First byte must be 0x55; 0xAA is invalid
  bool result = emu.parse_soyosource_inverter_emulator_byte_(0xAA);
  EXPECT_FALSE(result);
  EXPECT_EQ(emu.last_function, 0xFF);
}

TEST(WifiVersionTest, BadCrcRejected) {
  TestableSoyosourceInverterEmulator emu;
  emu.set_protocol_version(SOYOSOURCE_WIFI_VERSION);
  auto frame = wifi_frame(0x01);
  frame[11] ^= 0xFF;  // corrupt CRC
  emu.feed(frame);
  EXPECT_EQ(emu.last_function, 0xFF);
}

TEST(WifiVersionTest, StatusCommandDispatched) {
  TestableSoyosourceInverterEmulator emu;
  emu.set_protocol_version(SOYOSOURCE_WIFI_VERSION);
  emu.feed(wifi_frame(0x01));
  EXPECT_EQ(emu.last_function, 0x01);
  EXPECT_EQ(emu.last_data.size(), 12u);
}

TEST(WifiVersionTest, StatusAltCommandDispatched) {
  TestableSoyosourceInverterEmulator emu;
  emu.set_protocol_version(SOYOSOURCE_WIFI_VERSION);
  emu.feed(wifi_frame(0x00));
  EXPECT_EQ(emu.last_function, 0x00);
}

TEST(WifiVersionTest, SettingsCommandDispatched) {
  TestableSoyosourceInverterEmulator emu;
  emu.set_protocol_version(SOYOSOURCE_WIFI_VERSION);
  emu.feed(wifi_frame(0x03));
  EXPECT_EQ(emu.last_function, 0x03);
}

TEST(WifiVersionTest, RebootCommandDispatched) {
  TestableSoyosourceInverterEmulator emu;
  emu.set_protocol_version(SOYOSOURCE_WIFI_VERSION);
  emu.feed(wifi_frame(0x11));
  EXPECT_EQ(emu.last_function, 0x11);
}

// Display version parsing

TEST(DisplayVersionTest, StatusCommandDispatched) {
  TestableSoyosourceInverterEmulator emu;
  emu.set_protocol_version(SOYOSOURCE_DISPLAY_VERSION);
  emu.feed(display_frame(0x01));
  EXPECT_EQ(emu.last_function, 0x01);
  EXPECT_EQ(emu.last_data.size(), 6u);
}

TEST(DisplayVersionTest, SettingsCommandDispatched) {
  TestableSoyosourceInverterEmulator emu;
  emu.set_protocol_version(SOYOSOURCE_DISPLAY_VERSION);
  emu.feed(display_frame(0x03));
  EXPECT_EQ(emu.last_function, 0x03);
}

TEST(DisplayVersionTest, BadCrcRejected) {
  TestableSoyosourceInverterEmulator emu;
  emu.set_protocol_version(SOYOSOURCE_DISPLAY_VERSION);
  auto frame = display_frame(0x01);
  frame[5] ^= 0xFF;
  emu.feed(frame);
  EXPECT_EQ(emu.last_function, 0xFF);
}

TEST(DisplayVersionTest, WifiFrameTooShortIgnored) {
  // A 12-byte WiFi frame fed to a display-version device: 6 bytes are parsed
  // as one display frame; the remaining 6 bytes start a new (invalid) parse.
  TestableSoyosourceInverterEmulator emu;
  emu.set_protocol_version(SOYOSOURCE_DISPLAY_VERSION);
  auto wf = wifi_frame(0x01);
  // WiFi frame bytes 0..5 happen to form a valid display frame only if CRC matches.
  // We just verify no crash and the parser stays consistent.
  emu.feed(wf);
  // No assertion on last_function -- just verify no crash.
}

// Buffer cleared after invalid byte

TEST(ParserStateTest, BufferResetAfterBadHeader) {
  TestableSoyosourceInverterEmulator emu;
  emu.set_protocol_version(SOYOSOURCE_WIFI_VERSION);
  emu.parse_soyosource_inverter_emulator_byte_(0xAA);  // bad header -> buffer cleared
  // After reset, a valid frame starting fresh should work
  emu.feed(wifi_frame(0x01));
  EXPECT_EQ(emu.last_function, 0x01);
}

}  // namespace esphome::soyosource_inverter_emulator::testing
