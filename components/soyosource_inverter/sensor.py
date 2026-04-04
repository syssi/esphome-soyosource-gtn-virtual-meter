import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_BATTERY_VOLTAGE,
    CONF_TEMPERATURE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    ICON_CURRENT_AC,
    ICON_EMPTY,
    STATE_CLASS_MEASUREMENT,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_HERTZ,
    UNIT_VOLT,
    UNIT_WATT,
)

from . import CONF_SOYOSOURCE_INVERTER_ID, SOYOSOURCE_INVERTER_COMPONENT_SCHEMA

DEPENDENCIES = ["soyosource_inverter"]

CODEOWNERS = ["@syssi"]

CONF_BATTERY_CURRENT = "battery_current"
CONF_BATTERY_POWER = "battery_power"
CONF_AC_VOLTAGE = "ac_voltage"
CONF_AC_FREQUENCY = "ac_frequency"
CONF_OPERATION_STATUS_ID = "operation_status_id"

ICON_OPERATION_STATUS = "mdi:heart-pulse"

SENSOR_DEFS = {
    CONF_OPERATION_STATUS_ID: {
        "unit_of_measurement": UNIT_EMPTY,
        "icon": ICON_OPERATION_STATUS,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_BATTERY_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_BATTERY_CURRENT: {
        "unit_of_measurement": UNIT_AMPERE,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_CURRENT,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_BATTERY_POWER: {
        "unit_of_measurement": UNIT_WATT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_POWER,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_AC_VOLTAGE: {
        "unit_of_measurement": UNIT_VOLT,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 0,
        "device_class": DEVICE_CLASS_VOLTAGE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_AC_FREQUENCY: {
        "unit_of_measurement": UNIT_HERTZ,
        "icon": ICON_CURRENT_AC,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_EMPTY,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
    CONF_TEMPERATURE: {
        "unit_of_measurement": UNIT_CELSIUS,
        "icon": ICON_EMPTY,
        "accuracy_decimals": 1,
        "device_class": DEVICE_CLASS_TEMPERATURE,
        "state_class": STATE_CLASS_MEASUREMENT,
    },
}

CONFIG_SCHEMA = SOYOSOURCE_INVERTER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): sensor.sensor_schema(**kwargs)
        for key, kwargs in SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_INVERTER_ID])
    for key in SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
