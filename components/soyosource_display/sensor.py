import esphome.codegen as cg
from esphome.components import sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_BATTERY_VOLTAGE,
    ICON_COUNTER,
    DEVICE_CLASS_ENERGY,
    CONF_TEMPERATURE,
    STATE_CLASS_TOTAL_INCREASING,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_VOLTAGE,
    ENTITY_CATEGORY_DIAGNOSTIC,
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

from . import CONF_SOYOSOURCE_DISPLAY_ID, SoyosourceDisplay

DEPENDENCIES = ["soyosource_display"]

CODEOWNERS = ["@syssi"]

CONF_BATTERY_CURRENT = "battery_current"
CONF_BATTERY_POWER = "battery_power"
CONF_AC_VOLTAGE = "ac_voltage"
CONF_AC_FREQUENCY = "ac_frequency"
CONF_ERROR_BITMASK = "error_bitmask"
CONF_OPERATION_MODE_ID = "operation_mode_id"
CONF_OPERATION_STATUS_ID = "operation_status_id"
CONF_TOTAL_ENERGY = "total_energy"
CONF_OUTPUT_POWER = "output_power"

ICON_ERROR_BITMASK = "mdi:alert-circle-outline"
ICON_OPERATION_MODE = "mdi:heart-pulse"
ICON_OPERATION_STATUS = "mdi:heart-pulse"

UNIT_KILO_WATT_HOURS = "kWh"

SENSORS = [
    CONF_BATTERY_VOLTAGE,
    CONF_BATTERY_CURRENT,
    CONF_BATTERY_POWER,
    CONF_AC_VOLTAGE,
    CONF_AC_FREQUENCY,
    CONF_TEMPERATURE,
    CONF_ERROR_BITMASK,
    CONF_OPERATION_MODE_ID,
    CONF_OPERATION_STATUS_ID,
    CONF_TOTAL_ENERGY,
    CONF_OUTPUT_POWER,
]

# pylint: disable=too-many-function-args
CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_DISPLAY_ID): cv.use_id(SoyosourceDisplay),
        cv.Optional(CONF_ERROR_BITMASK): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_ERROR_BITMASK,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_OPERATION_MODE_ID): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_OPERATION_MODE,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_OPERATION_STATUS_ID): sensor.sensor_schema(
            unit_of_measurement=UNIT_EMPTY,
            icon=ICON_OPERATION_STATUS,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_CURRENT): sensor.sensor_schema(
            unit_of_measurement=UNIT_AMPERE,
            icon=ICON_EMPTY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_CURRENT,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_BATTERY_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AC_VOLTAGE): sensor.sensor_schema(
            unit_of_measurement=UNIT_VOLT,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_VOLTAGE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_AC_FREQUENCY): sensor.sensor_schema(
            unit_of_measurement=UNIT_HERTZ,
            icon=ICON_CURRENT_AC,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_EMPTY,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
            unit_of_measurement=UNIT_CELSIUS,
            icon=ICON_EMPTY,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_TEMPERATURE,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
        cv.Optional(CONF_TOTAL_ENERGY): sensor.sensor_schema(
            unit_of_measurement=UNIT_KILO_WATT_HOURS,
            icon=ICON_COUNTER,
            accuracy_decimals=1,
            device_class=DEVICE_CLASS_ENERGY,
            state_class=STATE_CLASS_TOTAL_INCREASING,
        ),
        cv.Optional(CONF_OUTPUT_POWER): sensor.sensor_schema(
            unit_of_measurement=UNIT_WATT,
            icon=ICON_EMPTY,
            accuracy_decimals=0,
            device_class=DEVICE_CLASS_POWER,
            state_class=STATE_CLASS_MEASUREMENT,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_DISPLAY_ID])
    for key in SENSORS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_sensor")(sens))
