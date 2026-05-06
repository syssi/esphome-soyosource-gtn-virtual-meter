#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_inverter::testing {

// RS485 status frame (10 data bytes) recorded from
// display-version-rs485-status-frames-unique.txt line 19181.
// Full frame: 0x23 0x01 0x01 0x00 0x01 0x02 0x22 0x00 0x00 0x00 0xEC 0x64 0x02 0x05 0x88
//
// Decoded values:
//   operation_status: 0x01 (Startup)
//   battery_voltage:  54.6 V    battery_current: 0 A    battery_power: 0 W
//   ac_voltage:      236 V      ac_frequency:    50 Hz
//   temperature:      21.7 °C   fan_running:     false
static const std::vector<uint8_t> RS485_STATUS_STARTUP = {
    0x01,        // operation status: Startup
    0x02, 0x22,  // battery voltage raw: 546 × 0.1 = 54.6 V
    0x00, 0x00,  // battery current raw: 0 A
    0x00, 0xEC,  // AC voltage raw: 236 V
    0x64,        // AC frequency raw: 100 × 0.5 = 50 Hz
    0x02, 0x05,  // temperature raw: 517 → (517 − 300) × 0.1 = 21.7 °C
};

}  // namespace esphome::soyosource_inverter::testing
