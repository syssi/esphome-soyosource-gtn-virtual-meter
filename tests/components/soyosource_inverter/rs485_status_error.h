#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_inverter::testing {

// RS485 status frame (10 data bytes) from a broken/error-state device, documented in
// soyosource_inverter.cpp source comment ("Response of my broken soyosource").
// Full frame: 0x23 0x01 0x01 0x00 0x04 0x02 0xBD 0x00 0x46 0x00 0xEF 0x64 0x02 0x43 0x20
//
// Decoded values:
//   operation_status: 0x04 ("Error or battery mode?")
//   battery_voltage:  70.1 V    battery_current: 7.0 A    battery_power: 490.7 W
//   ac_voltage:      239 V      ac_frequency:    50 Hz
//   temperature:      27.9 °C   fan_running:     false
static const std::vector<uint8_t> RS485_STATUS_ERROR = {
    0x04,        // operation status: Error or battery mode?
    0x02, 0xBD,  // battery voltage raw: 701 × 0.1 = 70.1 V
    0x00, 0x46,  // battery current raw: 70 × 0.1 = 7.0 A
    0x00, 0xEF,  // AC voltage raw: 239 V
    0x64,        // AC frequency raw: 100 × 0.5 = 50 Hz
    0x02, 0x43,  // temperature raw: 579 → (579 − 300) × 0.1 = 27.9 °C
};

}  // namespace esphome::soyosource_inverter::testing
