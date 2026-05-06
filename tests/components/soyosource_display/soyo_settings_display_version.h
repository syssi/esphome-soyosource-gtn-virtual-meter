#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_display::testing {

// WiFi/Soyo settings frame (15 bytes, SOF=0xA6) recorded from display-version-display-pdus.txt.
// Captured with Battery Constant Power mode selected.
//
// Decoded values:
//   operation_mode_setting: 0x02 (BatCP)       device_model: 1000 W / 220 V
//   device_type: "PV 26-56V / BAT 24V"          start_voltage: 30 V
//   shutdown_voltage: 28 V                      grid_voltage: 222 V
//   grid_frequency raw: 100 (50.0 Hz)           output_power_limit: 700 W
//   start_delay: 6 s
static const std::vector<uint8_t> SOYO_SETTINGS_DISPLAY_VERSION = {
    0xA6,        // header
    0x01, 0x86,  // unknown
    0x53,        // op_mode (high nibble=0x5 → BatCP) | function (low nibble=0x3 → SETTINGS)
    0x42,        // operation status bitmask
    0xD2,        // device model: (210 % 100) × 100 = 1000 W, >200 → 220 V
    0x18,        // device type: 24 → "PV 26-56V / BAT 24V"
    0x1E,        // start voltage: 30 V
    0x1C,        // shutdown voltage: 28 V
    0x00, 0xDE,  // grid voltage: 222 V
    0x64,        // grid frequency raw: 100
    0x46,        // output power limit: 70 × 10 = 700 W
    0x06,        // start delay: 6 s
    0x31,        // checksum
};

}  // namespace esphome::soyosource_display::testing
