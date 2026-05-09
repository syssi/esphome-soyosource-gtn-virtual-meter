#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_display::testing {

// A6 status frame (15 bytes) recorded from a GTN inverter with display+wifi port.
// Source: docs/display-and-wifi-version/README.md traffic capture.
//
// Decoded values (SOYOSOURCE_DISPLAY_AND_WIFI_VERSION):
//   operation_mode: 0x09 → "Battery Limit"   limiter_connected: true
//   operation_status: "Normal"                error_bitmask: 0
//   battery_voltage: 26.3 V                   battery_current: 4.3 A
//   battery_power: 26.3 × 4.3 = 113.09 W     output_power: 113.09 × 0.86956 ≈ 98.3 W
//   grid_voltage: 233 V                       ac_frequency: 50.0 Hz
//   temperature: 15.4 °C                      fan_running: false
//
// Under SOYOSOURCE_WIFI_VERSION the same frame would yield output_power = 0 W
// (bytes 1–2 = 0x0000).  The efficiency factor distinguishes the two protocols.
static const std::vector<uint8_t> SOYO_STATUS_DISPLAY_AND_WIFI_VERSION = {
    0xA6,        // header
    0x00, 0x00,  // bytes 1-2: not used for output power in this protocol version
    0x91,        // op_mode (high nibble=0x9 → Battery Limit) | function (low nibble=0x1 → STATUS)
    0x40,        // status bitmask: limiter connected (bit 6), no errors
    0x01, 0x07,  // battery voltage: 263 × 0.1 = 26.3 V
    0x00, 0x2B,  // battery current: 43 × 0.1 = 4.3 A
    0x00, 0xE9,  // grid voltage: 233 V
    0x64,        // grid frequency: 100 × 0.5 = 50.0 Hz
    0x01, 0xC6,  // temperature raw: 454 → (454 − 300) × 0.1 = 15.4 °C
    0xE7,        // checksum: 0xFF − sum(bytes[1..13]) = 0xE7
};

}  // namespace esphome::soyosource_display::testing
