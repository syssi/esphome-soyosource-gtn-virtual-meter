#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_display::testing {

// WiFi/Soyo status frame (15 bytes, SOF=0xA6) recorded from wifi-version-display-status-frames.txt.
//
// Decoded values (SOYOSOURCE_WIFI_VERSION):
//   operation_mode: 0x09 → "Battery Limit"   limiter_connected: true
//   operation_status: "Normal"                error_bitmask: 0
//   output_power: 900 W                       battery_voltage: 46.3 V
//   battery_current: 1.4 A                    battery_power: 64.82 W
//   grid_voltage: 247 V                       ac_frequency: 50.0 Hz
//   temperature: 22.1 °C                      fan_running: false
static const std::vector<uint8_t> SOYO_STATUS_WIFI_VERSION = {
    0xA6,  // header
    0x03, 0x84,  // output power: 900 W (WiFi version uses bytes 1-2)
    0x91,        // op_mode (high nibble=0x9 → Battery Limit) | function (low nibble=0x1 → STATUS)
    0x40,        // status bitmask: limiter connected (bit 6), no errors
    0x01, 0xCF,  // battery voltage: 463 × 0.1 = 46.3 V
    0x00, 0x0E,  // battery current: 14 × 0.1 = 1.4 A
    0x00, 0xF7,  // grid voltage: 247 V
    0x64,        // grid frequency: 100 × 0.5 = 50.0 Hz
    0x02, 0x09,  // temperature raw: 521 → (521 - 300) × 0.1 = 22.1 °C
    0x63,        // checksum
};

}  // namespace esphome::soyosource_display::testing
