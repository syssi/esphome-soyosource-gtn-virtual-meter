#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_display::testing {

// WiFi/Soyo status frame (15 bytes, SOF=0xA6) recorded from display-version-display-pdus.txt.
//
// Decoded values (SOYOSOURCE_DISPLAY_VERSION):
//   operation_mode: 0x09 → "Battery Limit"   limiter_connected: true
//   operation_status: "Normal"                error_bitmask: 0
//   battery_voltage: 24.8 V                   battery_current: 27.9 A
//   battery_power: 691.92 W                   output_power: 691.92 × 0.86956 ≈ 601.7 W
//   grid_voltage: 228 V                       ac_frequency: 50.0 Hz
//   temperature: 28.5 °C                      fan_running: false
static const std::vector<uint8_t> SOYO_STATUS_DISPLAY_VERSION = {
    0xA6,        // header
    0x02, 0x58,  // output power (unused by DISPLAY_VERSION parser, computed from battery)
    0x91,        // op_mode (high nibble=0x9 → Battery Limit) | function (low nibble=0x1 → STATUS)
    0x40,        // status bitmask: limiter connected (bit 6), no errors
    0x00, 0xF8,  // battery voltage: 248 × 0.1 = 24.8 V
    0x01, 0x17,  // battery current: 279 × 0.1 = 27.9 A
    0x00, 0xE4,  // grid voltage: 228 V
    0x64,        // grid frequency: 100 × 0.5 = 50.0 Hz
    0x02, 0x49,  // temperature raw: 585 → (585 - 300) × 0.1 = 28.5 °C
    0x31,        // checksum
};

}  // namespace esphome::soyosource_display::testing
