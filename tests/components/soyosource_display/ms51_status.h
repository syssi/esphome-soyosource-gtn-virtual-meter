#pragma once
#include <cstdint>
#include <vector>

namespace esphome::soyosource_display::testing {

// MS51 status frame (17 bytes, SOF=0x5A) recorded from wifi-dongle/nuvoton-esp8266-pdus.txt.
// Captured during inverter startup (battery not yet connected, standby).
//
// Decoded values:
//   operation_mode: 0x0D → "Battery Limit"   limiter_connected: false
//   operation_status: "Standby"              error_bitmask: 2 ("DC voltage too low")
//   battery_voltage: 0.0 V                    battery_current: 0.0 A
//   battery_power: 0.0 W                      output_power: 0 W
//   grid_voltage: 231 V                       ac_frequency: 50 Hz
//   total_energy: 0.0 kWh                     temperature: 25 °C
//   fan_running: false
static const std::vector<uint8_t> MS51_STATUS = {
    0x5A,        // header
    0x01,        // unknown (always 0x01)
    0xD1,        // op_mode (high nibble=0xD → Battery Limit) | function (low nibble=0x1 → STATUS)
    0x02,        // status bitmask: bit 1 set = DC voltage too low (standby)
    0x00, 0x00,  // battery voltage: 0 × 0.1 = 0.0 V
    0x00, 0x00,  // battery current: 0 × 0.1 = 0.0 A
    0x00, 0xE7,  // grid voltage: 231 V
    0x32,        // grid frequency: 50 Hz (integer, no coefficient)
    0x00, 0x00,  // output power: 0 W
    0x00, 0x00,  // total energy: 0 × 0.1 = 0.0 kWh
    0x19,        // temperature: 25 °C
    0xF9,        // checksum
};

}  // namespace esphome::soyosource_display::testing
