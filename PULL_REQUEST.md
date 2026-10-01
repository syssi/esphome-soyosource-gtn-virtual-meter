# Add ESP32 Home Assistant API Limiter Example (Optimized for Zero-Export)

## Summary of Changes

This PR introduces a dedicated, out-of-the-box example configuration for users who want to control their Soyosource GTN limiter via an **ESP32** connected directly to **Home Assistant via the Native API (`api:`)**, optimized for real-world zero-export performance:

- **New Example File**: `esp32-limiter-homeassistant-api-example.yaml`
- **Documentation**: Added the new setup to the "Supported setups" table in `README.md`.

---

## Motivation & Key Improvements over Existing Examples

While `esp32-limiter-example.yaml` is a great reference, it has a few defaults that require significant adjustments for the typical Home Assistant user:

1. **Dedicated Hardware UART2 (`GPIO16` / `GPIO17`)**:
   - `esp32-limiter-example.yaml` routes RS485 through `GPIO1` and `GPIO3` (UART0), forcing `logger: baud_rate: 0` and disabling USB serial monitoring.
   - This example moves RS485 to Hardware UART2 (`GPIO17` TX, `GPIO16` RX). This keeps the USB-C serial port completely free at 115200 baud for real-time ESPHome debugging, logs, and initial USB flashing.

2. **Native Home Assistant API out of the box**:
   - Instead of defaulting to MQTT with a specific OBIS smart-meter topic, this example directly integrates with Home Assistant's native API (`platform: homeassistant`) using standard substitution variables (`power_sensor_entity_id`).

3. **Optimized for Rapid Zero-Export Transient Response**:
   - Uses `power_demand_calculation: RESTART_ON_CROSSING_ZERO`. In real-world installations with high-draw cycling loads (like electric clothes dryers, ovens, or kettles), the default `NEGATIVE_MEASUREMENTS_REQUIRED` steps down gradually over several cycles, causing 4–6 seconds of continuous grid backfeed. `RESTART_ON_CROSSING_ZERO` immediately clamps demand to 0W on the first cycle export is detected, cutting the transient spike duration in half.
   - Uses `update_interval: 1s` (down from 3s) with a gentle `throttle: 1s` filter to filter out sub-second AC line jitter and prevent false 0W resets, while allowing fast power meters (such as Shelly 3EM / Shelly Pro 3EM) to push real-time grid changes smoothly.

4. **Cleaner Substitutions**:
   - Exposes `power_sensor_entity_id`, `power_sensor_name`, `tx_pin`, and `rx_pin` as clean substitutions at the top of the file for quick plug-and-play adoption.

---

## Hardware & Real-World Validation

- **Board**: Standard ESP32 DevKit (ESP-WROOM-32)
- **RS485 Transceiver**: Auto-direction TTL-to-RS485 transceiver module
- **Inverter**: Y&H / Soyosource GTN-1200W (Model: `01-GTN-1200W-48V-110V-L-US`)
- **Grid Power Sensor**: Shelly Pro 3EM monitoring split-phase grid power via Home Assistant
- **Results**: Verified sub-second reaction to load changes, stable zero-export regulation with positive buffer, and immediate recovery on crossing zero.
