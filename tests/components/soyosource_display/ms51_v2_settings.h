#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_display::testing {

// MS51 V2 settings frame (25 bytes, SOF=0x5A, frame_type=0x17) recorded from
// wifi-version-gtn2000lim48-dongle-traffic-capture.txt.
//
// Decoded values:
//   operation_mode_setting: 0x12 (Battery Limit, WiFi version)
//   device_model: 1500 W                       device_type: "PV 26-56V / BAT 24V"
//   start_voltage: 27 V                         shutdown_voltage: 25 V
//   output_power_limit: 50 W                    start_delay: 10 s
static const std::vector<uint8_t> MS51_V2_SETTINGS = {
    0x5A,  // header
    0x17,  // frame type: FRAME_TYPE_MS51_V2_SETTINGS
    0xD3,  // op_mode (high nibble=0xD → Battery Limit) | function (low nibble=0x3 → SETTINGS)
    0x42,  // operation status bitmask
    0xD7,  // device model: (215 % 100) × 100 = 1500 W
    0x18,  // device type: 24 → "PV 26-56V / BAT 24V"
    0x1B,  // start voltage: 27 V
    0x19,  // shutdown voltage: 25 V
    0x19,  // unknown
    0x08,  // unknown
    0x24,  // unknown (grid frequency?)
    0x05,  // output power limit: 5 × 10 = 50 W
    0x00,  // unknown
    0x0A,  // start delay: 10 s
    0x1A,  // unknown
    0x2D,  // unknown
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // V2 data (8 bytes)
    0x15,                                            // checksum
};

}  // namespace esphome::soyosource_display::testing
