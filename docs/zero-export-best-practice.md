# Zero-Export Best Practice: Dynamic Baseload Tracking

**Target Repository:** `syssi/esphome-soyosource-gtn-virtual-meter`  
**Contribution Type:** Documentation / Best Practice Guide / Architecture Reference  

---

> [!IMPORTANT]
> ### Design Objective & Operating Trade-Offs
> This guide is specifically written from the perspective of **Strict Zero-Export Optimization**—engineered for scenarios where avoiding utility revenue meter backfeed flags, uncredited reverse registers, or non-export interconnection limits (e.g., California Rule 21 Section M) is the primary operational constraint.
> 
> * **If your utility permits backfeed (e.g., Net Energy Metering)** or you are comfortable with brief reverse spikes, you may choose to run higher static limits to aggressively offset larger loads.
> * **Why Dynamic Baseload Tracking is still valuable:** Even if you are not sensitive to zero export, dynamically tracking the baseline stabilizes inverter operation, stops continuous thermal stress and choke chatter caused by chasing 10-second pulse loads, and ensures your battery capacity is conserved for continuous background consumption.

---

## 1. Executive Summary

When operating a Soyosource GTN-1000W / GTN-1200W grid-tie inverter in zero-export limiter mode using `esphome-soyosource-gtn-virtual-meter`, high-power cycling loads (such as non-inverter microwave ovens running on partial power, PWM-cycling induction cooktops, and oscillating space heaters) cause **severe, repeated backfeed spikes into the utility grid**.

This document outlines the root physical cause—an inherent slew-rate mismatch between digital load switching (<10 ms) and the inverter power-stage ramp-down curve (~4 seconds)—and details the **Dynamic Baseload Tracking** architecture. By allowing a slow supervisory automation to dynamically float `max_power_demand` to match the household's rolling 30–60 minute idle baseload (rather than maintaining a static high ceiling like 450W or 900W), **unpermitted grid export is reduced by >99%** without requiring hardware modifications or sacrificing continuous baseline energy savings.

---

## 2. The Problem: Pulse Loads vs. Inverter Slew Rates

### A. The Non-Inverter Microwave Signature
Standard residential non-inverter microwave ovens running at partial power settings (e.g., "50% Power" or "Defrost") do not vary magnetron power electronically. Instead, they operate via open-loop duty-cycle pulse-width modulation (PWM), switching the 1,200W+ magnetron relay fully ON and fully OFF:
* **Typical cycle:** ~9–11 seconds ON, ~18–20 seconds OFF (~31% duty cycle, ~29-second period).

```
Load (Watts)
1,250 W |      +----------+               +----------+
        |      |  (9 sec) |               |  (9 sec) |
    0 W +------+----------+---------------+----------+--------
               ^          ^
               |          |-- Magnetron snaps OFF in <10 milliseconds!
               |-- Magnetron turns ON
```

### B. The Closed-Loop Feedback Failure
When `max_power_demand` is left at a fixed high ceiling (e.g., 450W to 900W):
1. **Ramp-Up (t = 0s to 7s):** When the magnetron energizes, the power meter reports +1,250W. The virtual meter commands the inverter to ramp up toward its ceiling. Over ~4 to 5 seconds, the GTN inverter reaches full commanded output (e.g., 450W).
2. **Instant Cutoff (t = 9s):** The magnetron relay snaps open in under 10 milliseconds. Local household load on that phase instantly collapses back to the resting baseline (e.g., 70–80W).
3. **Slew-Rate Overshoot (t = 9s to 14s):** The GTN inverter's internal analog control loop and output filter choke cannot collapse power instantly; it exhibits an intrinsic ramp-down slew rate of ~4.0 seconds. For **~5 seconds on every single cycle**, the inverter injects 450W into a 75W load, forcing **~190W to 375W of excess power backward onto the utility grid**.
4. **Repetition:** During an 8–10 minute cooking session, this cycle repeats 15 to 25 times, effectively converting a zero-export system into an intermittent pulse generator.

### C. Regulatory & Metering Impact
Grid interconnection rules and smart revenue meters place strict boundaries on unintentional backfeed:
* **Interconnection Standards (e.g., California Rule 21 Section M / IEEE 1547):** Inadvertent export lasting between 2 and 60 seconds is permitted **only if it occurs fewer than two times in any 24-hour period**. Generating dozens of 5-second export pulses in an evening violates inadvertent export compliance.
* **Smart Meter Registers:** Modern utility digital revenue meters accumulate reverse Wh in dedicated non-volatile `Received` or `Reverse` registers. Even if your billing tariff does not penalize small net reverse energy over a 15-minute integration interval, repeated high-frequency reverse spikes can trip tamper or unauthorized generation flags.

---

## 3. The Solution: Dynamic Baseload Tracking

### Decoupling Fast Zero-Export from Slow Baseload Envelope
The GTN microinverter's primary economic objective in a zero-export battery system is **continuous domestic baseline offset** (offsetting 24/7 idle background draw during peak Time-Of-Use rates), rather than whole-home peak shaving.

Household baseload naturally varies over the 24-hour cycle:
* **Nighttime Sleep Window:** ~120W – 160W (refrigerator standby, network equipment, standby electronics).
* **Active Daytime & Evening:** ~220W – 280W (ambient lighting, computers, ventilation, entertainment).

Instead of maintaining a static 450W ceiling, a supervisory loop calculates the **rolling 45-minute 20th-percentile baseload** and dynamically floats `number.soyosource_limiter_max_power_demand` every 15 minutes.

### Why Dynamic Baseload Eliminates Overshoot
When `max_power_demand` dynamically hugs the true resting baseline (e.g., ~220W during the evening):
1. When a microwave or kettle switches on at 1,250W, the inverter is capped at 220W.
2. When the load suddenly cuts off, the inverter is already outputting ~220W into a home that continuously consumes ~220W.
3. **Net grid power remains ≥ 0W.** Reverse backfeed is virtually eliminated.
4. When a long continuous cooking load runs (such as an air fryer running steadily for 5 minutes), the inverter provides steady, chatter-free baseload displacement without overshoot.

---

## 4. Empirical Test Results: Reference Installation

The following empirical metrics were captured using 1-second telemetry during a dinner preparation session featuring cycling microwave use and continuous air fryer cooking.

**System Setup:**
* **Service:** Split-phase 120V / 240V utility grid service monitored via whole-home energy meter (Shelly Pro 3EM).
* **Storage & Inverter:** 48V (51.2V nominal) LiFePO4 battery bank paired with a Soyosource GTN-1200W grid-tie inverter controlled via RS-485 by an ESP32 running `esphome-soyosource-gtn-virtual-meter`.
* **Measured Household Baseload:** Nighttime idle ~140W; active evening baseline ~220–260W.

| Limiter Ceiling Strategy | Peak Reverse Spike | Export Spike Count | Total Energy Spilled | Baseload Offset Delivered |
| :--- | :--- | :--- | :--- | :--- |
| **Static Ceiling (450 W)** | **-192.6 W** | 70 events | **4.88 Wh** | 281.6 Wh |
| **Static Ceiling (300 W)** | -75.6 W | 50 events | 0.75 Wh | 241.0 Wh |
| **Static Ceiling (250 W)** | -43.7 W | 17 events | 0.18 Wh | 226.6 Wh |
| **Dynamic Baseload (220 W)** | **-25.8 W** | **12 events** | **0.05 Wh** | **217.6 Wh (78% captured)** |
| **Dynamic Baseload (200 W)** | **-13.8 W** | **7 events** | **0.01 Wh** | **210.6 Wh (75% captured)** |

### Key Observations
1. **99.0% Backfeed Reduction:** Dropping from a static 450W ceiling to dynamic baseload tracking reduced total spilled reverse energy from 4.88 Wh down to 0.05 Wh, and peak reverse power from -192.6W down to a negligible -25.8W (safely within panel-phase absorption margins).
2. **High Energy Capture (78%):** Despite heavily restricting pulse-load overshoots, the inverter still delivered 78% of total potential energy offset, covering 100% of the household's steady continuous baseline throughout the entire period.
3. **Zero Inverter Stress:** Eliminates rapid thermal cycling and output choke chatter caused by chasing 10-second pulses.

---

## 5. Home Assistant Implementation Guide

This implementation requires **no ESPHome firmware recompilation** and works out of the box with existing `esphome-soyosource-gtn-virtual-meter` installations.

### Step 1: Gross Power Sensor
Ensure you have a sensor representing instantaneous whole-home power consumption. If you already have a whole-house consumption sensor (`sensor.power_consumption`), use that directly. If your panel meter only reports net grid import, combine net import with current inverter backfeed:

```yaml
template:
  - sensor:
      - name: "Power Consumption"
        unique_id: power_consumption
        device_class: power
        state_class: measurement
        unit_of_measurement: "W"
        state: >
          {{ [0, (states('sensor.grid_net_import_power') | float(0) + 
                  states('sensor.inverter_power_output') | float(0))] | max | round(1) }}
```

### Step 2: Statistics Helper (Rolling Baseload Extraction)
Create a Statistics Helper to calculate the 20th percentile over a 45-minute window. The 20th percentile is ideal because it ignores transient cooking and appliance spikes while accurately tracking the true resting floor.

In Home Assistant (**Settings > Devices & Services > Helpers > Add Helper > Statistics**):
* **Name:** `House Baseload 45m`
* **Input Entity:** `sensor.power_consumption`
* **Characteristic:** `percentile`
* **Percentile value:** `20`
* **Max age:** `00:45:00`
* **Sampling size:** `2000`

> [!NOTE]
> **Buffer Sizing for Real-Time Telemetry:**  
> Whole-home power meters (such as the Shelly Pro 3EM) report measurements every 1–2 seconds, generating ~1,350 to 2,700 samples in a 45-minute window. Home Assistant's Statistics integration caps its internal FIFO circular buffer to whichever limit is reached first: `max_age` or `sampling_size`. Setting `sampling_size` too low (such as 100) causes the buffer to overflow in under 3 minutes, silently truncating the time window and allowing cycling appliances to falsely inflate the baseload. A `sampling_size` of `2000` ensures the full 45-minute window is retained.

### Step 3: Supervisory Ceiling Automation
Create an automation that runs periodically (e.g., every 15 minutes) and on Home Assistant startup. It clamps the ceiling between a nighttime floor (e.g., 150W) and a hardware safety ceiling (e.g., 450W), adding a small offset (+15W) to track baseline load cleanly.

```yaml
automation:
  - alias: "Solar: Adaptive Baseload Limiter Ceiling"
    description: "Periodically adjusts Soyosource limiter ceiling to match rolling household baseline"
    mode: single
    triggers:
      - trigger: time_pattern
        minutes: "/15"
      - trigger: homeassistant
        event: start
    conditions:
      # Optional: ensure high-draw continuous heating or EV charging guards are off
      - condition: state
        entity_id: binary_sensor.space_heating_active
        state: "off"
    actions:
      - action: number.set_value
        target:
          entity_id: number.soyosource_limiter_max_power_demand
        data:
          value: >
            {# Bounds: adjust the floor (150W) and ceiling (450W) to your setup #}
            {{ [150, [450, (states('sensor.house_baseload_45m') | float(200) + 15) | round(0)] | min] | max }}
```

---

## 6. Slew Rate Simulator Tool

To visualize the interaction between load cycling, hardware slew rates, and ceiling limits, a browser-based simulator is included:
* **Interactive Tool:** [slew_simulator.html](slew_simulator.html)
* **Capabilities:** Adjust load pulse widths, duty cycles, inverter ramp-up/ramp-down slew rates, and static vs. adaptive ceilings in real time with instant backfeed Wh and peak reverse power calculations.

---

## 7. Next Steps & Native Firmware Integration

While this Home Assistant supervisory package resolves the backfeed problem immediately for all current users, in an upcoming pull request we plan to add native C++ adaptive baseload tracking directly into `soyosource_virtual_meter.cpp`. This will allow the ESP32 to maintain the circular baseline buffer on-device, providing zero-export protection even when the Home Assistant server or network is offline.
