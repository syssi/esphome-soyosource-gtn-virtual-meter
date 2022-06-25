# WiFi Dongle V1.7

## USB connector

```
o GND ──── GND
o RX  ──── 150 Ohm + RXD_1 (P0.2)
o TX  ──── 150 Ohm + TXD_1 (P1.6)
o 5V  ──── AMS1117
```

## Nuvoton MS51FB9AE

```
# Pin header (PCB back)

      ┌─────────────────────┐
┌─────┘                 M   │
│         [AMS]         S   │
│                       5   │
└─────┐ ooooo           1   │
      └─────────────────────┘
        │││││
        │││││
        ││││└─ Pin1  3.3V
        │││└── Pin2  TXD_1 (P1.6)
        ││└─── Pin3  RXD_1 (P0.2)
        │└──── Pin4  nRESET (P2.0)
        └───── Pin5  GND


# Nuvoton MS51FB9AE

            ┌───────────┐
         T0 o           o PWM0_CH3
        TXD o           o PWM0_CH5
        RXD o           o RXD_1
     nRESET o           o
            o           o
            o           o
        VSS o           o
      TXD_1 o           o
        VDD o           o
            o           o PWM0_CH1
            └───────────┘

       T0 (P0.5) ──── BUTTON-
      TXD (P0.6) ──── RXD0 (GPIO3, ESP8266)
      RXD (P0.7) ──── TXD0 (GPIO1, ESP8266)
   nRESET (P2.0) ──── Pin header
    TXD_1 (P1.6) ──── TX (USB) + 150Ohm
    RXD_1 (P0.2) ──── RX (USB) + 150Ohm
 PWM0_CH3 (P0.4) ──── LED- D2
 PWM0_CH5 (P0.3) ──── LED- D4
 PWM0_CH1 (P1.4) ──── LED- D3
             VDD ──── AMS1117 OUT (3.3V)
             VSS ──── GND

```

## ESP8266MOD

```

           ┌──────────┐
           │ ┌─────── │
           │          │
       RST o          o TXD0 (GPIO1)
      ADC0 o          o RXD0 (GPIO3)
    ENABLE o          o
           o          o GPIO4
    GPIO14 o          o GPIO0 (FLASH, not connected)
           o          o
           o          o GPIO15
       VCC o          o GND
           └──oooooo──┘


           RST ──── 10kOhm pull-up
          ADC0 ──── NC
        ENABLE ──── 10kOhm pull-up
        GPIO14 ──── NC
           VCC ──── VCC 3.3V
          TXD0 ──── RXD (P0.7)
          RXD0 ──── TXD (P0.6)
         GPIO4 ──── NC
         GPIO0 ──── NC
        GPIO15 ──── 10kOhm pull-down
           GND ──── GND

```

## How to backup the ESP8266

1. Solder a wire to `GPIO1 (TXD0)`, `GPIO3 (RXD0)` and `GPIO0 (FLASH)` of the ESP
2. Solder a wire to the `GND (Pin5)`, `nRESET (Pin4)` and `3.3V (Pin1)` of the pin header
3. Connect `GND`, `RXD0`, `TXD0` and `3.3V` to a USB-to-TTL adapter
4. Pull-down `nRESET` to `GND` (connect Pin5 + Pin4)
5. Pull-down `GPIO0` of the ESP to `GND` to trigger the `download mode` on boot-up
6. Plug the USB-to-TTL adapter into your computer
7. Use esptool to read the flash

If LED `D2`, `D3` or `D4` turns on permanently `nRESET` isn't connected properly to `GND`. It's important
to stop the MS51 be pulling down `nRESET`. If you communicate with the ESP8266 the blue LED next
to the antenna will indicate the traffic.

```
$ esptool.py --port /dev/ttyUSB0 --baud 115200 -c esp8266 flash_id
esptool.py v3.3
Serial port /dev/ttyUSB0
Connecting...
Chip is ESP8266EX
Features: WiFi
Crystal is 26MHz
MAC: 40:91:51:51:d2:57
Stub is already running. No upload is necessary.
Manufacturer: 20
Device: 4016
Detected flash size: 4MB
Hard resetting via RTS pin...

$ esptool.py --port /dev/ttyUSB0 --baud 115200 read_flash 0x00000 0x400000 backup.img
esptool.py v3.3
Serial port /dev/ttyUSB0
Connecting....
Detecting chip type... Unsupported detection protocol, switching and trying again...
Connecting...
Detecting chip type... ESP8266
Chip is ESP8266EX
Features: WiFi
Crystal is 26MHz
MAC: 40:91:51:51:d2:57
Uploading stub...
Running stub...
Stub running...
4194304 (100 %)
Read 4194304 bytes at 0x0 in 378.7 seconds (88.6 kbit/s)...
Hard resetting via RTS pin...
```
