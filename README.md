# esphome-soyosource-gtn-virtual-meter

ESPHome component to simulate the power meter to control the Soyosource GTN1200 limiter

![Lovelace entities card](lovelace-entities-card.png "lovelace entities card")

## Requirements

* [ESPHome 1.18.0 or higher](https://github.com/esphome/esphome/releases).

## Schematics

```
               RS485                      UART
┌─────────┐              ┌──────────┐                ┌─────────┐
│         │              │          │<----- RX ----->│         │
│         │<-----B- ---->│  RS485   │<----- TX ----->│ ESP32/  │
│ GTN1200 │<---- A+ ---->│  to TTL  │<----- GND ---->│ ESP8266 │
│         │<--- GND ---->│  module  │<--- 5V VCC --->│         │<-- 5V VCC
│         │              │          │                │         │<-- GND
└─────────┘              └──────────┘                └─────────┘

```

## Installation

You can install this component with [ESPHome external components feature](https://esphome.io/components/external_components.html) like this:
```yaml
external_components:
  - source: github://syssi/esphome-soyosource-gtn-virtual-meter@main
```

## Configuration

```yaml
substitutions:
  name: soyosource_gtn_virtual_meter

esphome:
  name: ${name}
  platform: ESP32
  board: esp-wrover-kit

external_components:
  - source: github://syssi/esphome-soyosource-gtn-virtual-meter@main
    refresh: 0s

wifi:
  ssid: !secret wifi_ssid
  password: !secret wifi_password

ota:
api:

logger:
  baud_rate: 0

uart:
  baud_rate: 4800
  tx_pin: GPIO1
  rx_pin: GPIO3

soyosource_modbus:

soyosource_virtual_meter:
  # the state of this sensor (instantaneous power in watt) is used as source
  power_id: powermeter
  min_power_demand: 0
  max_power_demand: 1000
  # A positive buffer value (10) tries to avoid exporting power to the grid (demand - 10 watts)
  # A negative buffer value (-10) exports power to the grid (demand + 10 watts)
  buffer: 10

sensor:
  - platform: soyosource_virtual_meter
    power_demand:
      name: "${name} power demand"

  - platform: soyosource_inverter
    battery_voltage:
      name: "${name} battery voltage"
    battery_current:
      name: "${name} battery current"
    battery_power:
      name: "${name} battery power"
    ac_voltage:
      name: "${name} ac voltage"
    ac_frequency:
      name: "${name} ac frequency"
    temperature:
      name: "${name} temperature"
    error_bits:
      name: "${name} error bits"

  # import smartmeter reading from homeassistant f.e.
  - platform: homeassistant
    id: powermeter
    name: "${name} smartmeter instantaneous power"
    entity_id: sensor.firstfloor_smartmeter_instantaneous_power
```

For a more advanced setup take a look at the [advanced-multiple-uarts.yaml](advanced-multiple-uarts.yaml).

## Known issues

None.

## References

* https://github.com/drcross/virtual-meter
* https://www.photovoltaikforum.com/thread/148552-g%C3%BCnstiger-1200w-grid-tie-inverter-mit-limiter-sensor-von-soyo-source-%C3%A4hnlich-gti/?pageNo=4
* https://secondlifestorage.com/index.php?threads/limiter-inverter-with-rs485-load-setting.7631/

