#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_display::testing {

// WiFi/Soyo settings frame (15 bytes, SOF=0xA6) recorded from wifi-version-display-pdus.txt.
// Captured with PV mode selected.
//
// Decoded values:
//   operation_mode_setting: 0x01 (PV)         device_model: 1200 W / 220 V
//   device_type: "PV 55-90V / BAT 48V"         start_voltage: 48 V
//   shutdown_voltage: 45 V                     grid_voltage: 250 V
//   grid_frequency raw: 100 (50.0 Hz)          output_power_limit: 900 W
//   start_delay: 6 s
static const std::vector<uint8_t> SOYO_SETTINGS_WIFI_VERSION = {
    0xA6,        // header
    0x00, 0x00,  // unknown
    0x63,        // op_mode (high nibble=0x6 → PV) | function (low nibble=0x3 → SETTINGS)
    0x02,        // operation status bitmask
    0xD4,        // device model: (212 % 100) × 100 = 1200 W, >200 → 220 V
    0x30,        // device type: 48 → "PV 55-90V / BAT 48V"
    0x30,        // start voltage: 48 V
    0x2D,        // shutdown voltage: 45 V
    0x00, 0xFA,  // grid voltage: 250 V
    0x64,        // grid frequency raw: 100
    0x5A,        // output power limit: 90 × 10 = 900 W
    0x06,        // start delay: 6 s
    0x7B,        // checksum
};

}  // namespace esphome::soyosource_display::testing
