#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_display::testing {

// MS51 settings frame (17 bytes, SOF=0x5A) recorded from wifi-dongle/nuvoton-esp8266-pdus.txt.
//
// Decoded values:
//   operation_mode_setting: 0x12 (Battery Limit, WiFi version)
//   device_model: 1200 W / 220 V              device_type: "PV 55-90V / BAT 48V"
//   start_voltage: 49 V                        shutdown_voltage: 47 V
//   grid_voltage: 231 V                        grid_frequency raw: 100
//   output_power_limit: 900 W                  start_delay: 6 s
static const std::vector<uint8_t> MS51_SETTINGS = {
    0x5A,        // header
    0x01,        // unknown (always 0x01)
    0xD3,        // op_mode (high nibble=0xD → Battery Limit) | function (low nibble=0x3 → SETTINGS)
    0x02,        // operation status bitmask
    0xD4,        // device model: (212 % 100) × 100 = 1200 W, >200 → 220 V
    0x30,        // device type: 48 → "PV 55-90V / BAT 48V"
    0x31,        // start voltage: 49 V
    0x2F,        // shutdown voltage: 47 V
    0x00, 0xE7,  // grid voltage: 231 V
    0x64,        // grid frequency raw: 100
    0x5A,        // output power limit: 90 × 10 = 900 W
    0x00,        // unknown
    0x06,        // start delay: 6 s
    0x37,        // spec: minimum PV voltage (55 V)
    0x5A,        // spec: maximum PV voltage (90 V)
    0x89,        // checksum
};

}  // namespace esphome::soyosource_display::testing
