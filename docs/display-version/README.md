# Soyosource Display Version

## Internal Display Port

```

        │        │   R│ │
        │   CPU  │   S│ │
        └────────┘   4│ │
                     8│ │
            oooo     4│ │
            Disp        │
 ────────┐  ┌───┐  ┌────┘
         └──┘   └──┘ ││
          B+     B-

            ││││
            ││││
            ││││
            │││└── RXD
            ││└─── TXD
            │└──── GND
            └───── 5V
```

![Internal display port](internal-display-port.jpg "Internal display port")

## Schematics

The display port connector is called SH 1.0mm, 4 Pin. Please remove the original display port cable. The pinout of the second connector is reversed. The digital isolator module is a ADUM1201.

![Isolated data cable](isolated-data-cable.jpg "Isolated data cable")

| SH 1.0mm, 4 Pin connector                                                        | Wemos D1 mini                                          |
|:--------------------------------------------------------------------------------:|:------------------------------------------------------:|
| ![Display port connector](display-port-connector.jpg "Display port connector")   | ![ESP8266 pinout](esp8266-pinout.jpg "ESP8266 pinout") |
