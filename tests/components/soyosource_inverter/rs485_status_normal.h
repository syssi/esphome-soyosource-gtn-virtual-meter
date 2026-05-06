#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_inverter::testing {

// RS485 status frame (10 data bytes, data[0..9] = full_frame[4..13]) recorded from
// https://secondlifestorage.com/index.php?threads/limiter-inverter-with-rs485-load-setting.7631/
// and documented in soyosource_inverter.cpp source comment.
// Full frame: 0x23 0x01 0x01 0x00 0x00 0x01 0xDB 0x00 0xA1 0x00 0xDD 0x64 0x02 0xCA 0x01
//
// Decoded values:
//   operation_status: 0x00 (Normal)
//   battery_voltage:  47.5 V    battery_current: 16.1 A    battery_power: 765.25 W
//   ac_voltage:      221 V      ac_frequency:     50 Hz
//   temperature:      41.4 °C   fan_running:      false
static const std::vector<uint8_t> RS485_STATUS_NORMAL = {
    0x00,        // operation status: Normal
    0x01, 0xDB,  // battery voltage raw: 475 × 0.1 = 47.5 V
    0x00, 0xA1,  // battery current raw: 161 × 0.1 = 16.1 A
    0x00, 0xDD,  // AC voltage raw: 221 V
    0x64,        // AC frequency raw: 100 × 0.5 = 50 Hz
    0x02, 0xCA,  // temperature raw: 714 → (714 − 300) × 0.1 = 41.4 °C
};

}  // namespace esphome::soyosource_inverter::testing
