# esphome-soyosource-gtn-virtual-meter

![GitHub actions](https://github.com/syssi/esphome-soyosource-gtn-virtual-meter/actions/workflows/ci.yaml/badge.svg)
![GitHub stars](https://img.shields.io/github/stars/syssi/esphome-soyosource-gtn-virtual-meter)
![GitHub forks](https://img.shields.io/github/forks/syssi/esphome-soyosource-gtn-virtual-meter)
![GitHub watchers](https://img.shields.io/github/watchers/syssi/esphome-soyosource-gtn-virtual-meter)
[!["Buy Me A Coffee"](https://img.shields.io/badge/buy%20me%20a%20coffee-donate-yellow.svg)](https://www.buymeacoffee.com/syssi)

ESPHome component to simulate the current clamp to control the Soyosource GTN1200 limiter

![Lovelace entities card](lovelace-entities-card.png "lovelace entities card")


## Supported devices

* GTN-1000LIM24, GTN-1000LIM36, GTN-1000LIM48, GTN-1000LIM72, GTN-1000LIM96
* GTN-1200LIM48, GTN-1200LIM72, GTN-1200LIM96
* GTN-1000LIM24W, GTN-1000LIM36W, GTN-1000LIM48W, GTN-1000LIM72W, GTN-1000LIM96W
* GTN-1200LIM48W, GTN-1200LIM72W, GTN-1200LIM96W

It looks like there is no GTW (waterproof) version of the device with limiter / RS485 support.

## Supported setups

| Configuration example                                                                  | Description                                                                                                                           |
|----------------------------------------------------------------------------------------|:--------------------------------------------------------------------------------------------------------------------------------------|
| [esp32-example.yaml](esp32-example.yaml)                                               | Monitor[^1] the inverter and control the power output on demand **both** via RS485 using a ESP32                                      |
| [esp8266-example.yaml](esp8266-example.yaml)                                           | Monitor[^1] the inverter and control the power output on demand **both** via RS485 using a ESP8266                                    |
| [esp32-limiter-example.yaml](esp32-limiter-example.yaml)                               | Control the power output only on demand via RS485 using a ESP32                                                                       |
| [esp8266-limiter-example.yaml](esp8266-limiter-example.yaml)                           | Control the power output only on demand via RS485 using a ESP8266                                                                     |
| [esp8266-display-display-version-example.yaml](esp8266-display-display-version-example.yaml)                  | Monitor and control the inverter (Display version) using the **internal display port**                                                  |
| [esp8266-display-display-version-limiter-example.yaml](esp8266-display-display-version-limiter-example.yaml)  | Monitor and control the inverter (Display version) using the **internal display port** and control the power output on demand via RS485 |
| [esp8266-display-wifi-version-example.yaml](esp8266-display-wifi-version-example.yaml)                        | Monitor and control the inverter (WiFi version) using the **display port**                                                              |
| [esp8266-display-wifi-version-limiter-example.yaml](esp8266-display-wifi-version-limiter-example.yaml)        | Monitor and control the inverter (WiFi version) using the **display port** and control the power output on demand via RS485             |
| [esp8266-wifi-dongle-example.yaml](esp8266-wifi-dongle-example.yaml)                   | Monitor and control the inverter using the **OEM WiFi dongle** with a custom firmware                                                 |
| [esp8266-wifi-dongle-example-web-only.yaml](esp8266-wifi-dongle-example-web-only.yaml) | Monitor and control the inverter using the **OEM WiFi dongle** with a custom firmware providing a nice **web interface**              |
| [esp8266-wifi-dongle-limiter-example.yaml](esp8266-wifi-dongle-limiter-example.yaml)   | Monitor and control the inverter using the **OEM WiFi dongle** and control the power output on demand via RS485                       |
| [esp32-multiple-uarts-example.yaml](esp32-multiple-uarts-example.yaml)                 | Monitor[^1] **multiple inverters** and control the power output on demand **both** via RS485                                          |

## Requirements

* [ESPHome 2022.5.0 or higher](https://github.com/esphome/esphome/releases).
* RS485-to-TTL module (`HW-0519` f.e.)
* Generic ESP32 or ESP8266 board
* Required [Soyosource inverter settings](docs/soyosource-settings.jpg "Soyosource config panel")
  * Battery CP Grid `N`
  * Bat CP Mode Power `Upper power limit`
  * Bat AutoLimit Grid `Y`

## Schematics

<a href="https://raw.githubusercontent.com/syssi/esphome-soyosource-gtn-virtual-meter/main/images/circuit.jpg" target="_blank">
<img src="https://raw.githubusercontent.com/syssi/esphome-soyosource-gtn-virtual-meter/main/images/circuit_thumbnail.jpg" width="60%">
</a>

```
               RS485                      UART
┌─────────┐              ┌──────────┐                ┌─────────┐
│         │              │          │<----- RX ----->│         │
│         │<-----B- ---->│  RS485   │<----- TX ----->│ ESP32/  │
│ GTN1200 │<---- A+ ---->│  to TTL  │<----- GND ---->│ ESP8266 │
│         │<--- GND ---->│  module  │<-- 3.3 VCC --->│         │<-- VCC
│         │              │          │                │         │<-- GND
└─────────┘              └──────────┘                └─────────┘

```

Please make sure to power the RS485 module with 3.3V because it affects the TTL (transistor-transistor logic) voltage between RS485 module and ESP.

## Installation

You can install this component with [ESPHome external components feature](https://esphome.io/components/external_components.html) like this:
```yaml
external_components:
  - source: github://syssi/esphome-soyosource-gtn-virtual-meter@main
```

or just use the `esp32-example.yaml` / `esp8266-example.yaml` as proof of concept:

```bash
# Install esphome
pip3 install esphome

# Clone this external component
git clone https://github.com/syssi/esphome-soyosource-gtn-virtual-meter.git
cd esphome-soyosource-gtn-virtual-meter

# Create a secrets.yaml containing some setup specific secrets
cat > secrets.yaml <<EOF
wifi_ssid: MY_WIFI_SSID
wifi_password: MY_WIFI_PASSWORD

mqtt_host: MY_MQTT_HOST
mqtt_username: MY_MQTT_USERNAME
mqtt_password: MY_MQTT_PASSWORD
EOF

# Validate the configuration, create a binary, upload it, and start logs
# If you use a esp8266 run the esp8266-examle.yaml
esphome run esp32-example.yaml

```

For a more advanced setup take a look at the [esp32-multiple-uarts-example.yaml](esp32-multiple-uarts-example.yaml).

## Known issues

None.

## Debugging

If this component doesn't work out of the box for your device please update your configuration to enable the debug output of the UART component and increase the log level to the see outgoing and incoming serial traffic:

```
logger:
  level: DEBUG
  logs:
    api.service: WARN
    ota: WARN
    sensor: DEBUG

uart:
  baud_rate: 4800
  tx_pin: GPIO1
  rx_pin: GPIO3
  debug:
    direction: BOTH
```

## References

* https://github.com/drcross/virtual-meter
* https://www.photovoltaikforum.com/thread/148552-g%C3%BCnstiger-1200w-grid-tie-inverter-mit-limiter-sensor-von-soyo-source-%C3%A4hnlich-gti/?pageNo=4
* https://secondlifestorage.com/index.php?threads/limiter-inverter-with-rs485-load-setting.7631/
* https://github.com/PepeTheFroggie/Soyosource-GridTie-inverter-24V-LCD-replacement

[^1]: Some devices doesn't respond to the status request (`0x24 0x00 0x00 0x00 0x00 0x00 0x00 0x00`) via RS485. The reason is unknown. This has no effect on the limiter feature. The inverter processes the power demand requests silently. There is a new hardware version since 2022 (purple mainboard / fw version `STC8-2022-218`) which probably doesn't respond to requests anymore. The 2021 version (blue mainboard & green mainboard, fw version `2021-301`) responds for some people and for some not. ([#48](https://github.com/syssi/esphome-soyosource-gtn-virtual-meter/issues/48))
