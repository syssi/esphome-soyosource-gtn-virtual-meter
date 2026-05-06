#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_inverter::testing {

// RS485 status frames (10 data bytes each, data[0..9] = full_frame[4..13]).
// Sources: soyosource_inverter.cpp inline comments (secondlifestorage.com forum
// capture and author's own device), display-version-rs485-status-frames-unique.txt.

// Normal operation with output power.
// Full frame: 0x23 0x01 0x01 0x00 0x00 0x01 0xDB 0x00 0xA1 0x00 0xDD 0x64 0x02 0xCA 0x01
// operation_status: 0x00 (Normal)
// battery_voltage: 475 * 0.1 = 47.5 V
// battery_current: 161 * 0.1 = 16.1 A
// battery_power: 47.5 * 16.1 = 765.25 W
// ac_voltage: 221 V, ac_frequency: 50 Hz
// temperature: (714 - 300) * 0.1 = 41.4 C, fan_running: false
static const std::vector<uint8_t> RS485_STATUS_NORMAL = {
    0x00,        // operation status: Normal
    0x01, 0xDB,  // battery voltage raw: 475
    0x00, 0xA1,  // battery current raw: 161
    0x00, 0xDD,  // AC voltage raw: 221 V
    0x64,        // AC frequency raw: 100 (100 * 0.5 = 50 Hz)
    0x02, 0xCA,  // temperature raw: 714 ((714 - 300) * 0.1 = 41.4 C)
};

// Error / battery mode state (author's broken device).
// Full frame: 0x23 0x01 0x01 0x00 0x04 0x02 0xBD 0x00 0x46 0x00 0xEF 0x64 0x02 0x43 0x20
// operation_status: 0x04 (Error or battery mode?)
// battery_voltage: 701 * 0.1 = 70.1 V
// battery_current: 70 * 0.1 = 7.0 A
// battery_power: 70.1 * 7.0 = 490.7 W
// ac_voltage: 239 V, ac_frequency: 50 Hz
// temperature: (579 - 300) * 0.1 = 27.9 C, fan_running: false
static const std::vector<uint8_t> RS485_STATUS_ERROR = {
    0x04,        // operation status: Error or battery mode?
    0x02, 0xBD,  // battery voltage raw: 701
    0x00, 0x46,  // battery current raw: 70
    0x00, 0xEF,  // AC voltage raw: 239 V
    0x64,        // AC frequency raw: 100 (50 Hz)
    0x02, 0x43,  // temperature raw: 579 ((579 - 300) * 0.1 = 27.9 C)
};

// Startup state, no output current.
// Full frame: 0x23 0x01 0x01 0x00 0x01 0x02 0x22 0x00 0x00 0x00 0xEC 0x64 0x02 0x05 0x88
// Source: display-version-rs485-status-frames-unique.txt line 19181
// operation_status: 0x01 (Startup)
// battery_voltage: 546 * 0.1 = 54.6 V, battery_current: 0 A
// ac_voltage: 236 V, ac_frequency: 50 Hz
// temperature: (517 - 300) * 0.1 = 21.7 C, fan_running: false
static const std::vector<uint8_t> RS485_STATUS_STARTUP = {
    0x01,        // operation status: Startup
    0x02, 0x22,  // battery voltage raw: 546
    0x00, 0x00,  // battery current raw: 0
    0x00, 0xEC,  // AC voltage raw: 236 V
    0x64,        // AC frequency raw: 100 (50 Hz)
    0x02, 0x05,  // temperature raw: 517 ((517 - 300) * 0.1 = 21.7 C)
};

}  // namespace esphome::soyosource_inverter::testing
