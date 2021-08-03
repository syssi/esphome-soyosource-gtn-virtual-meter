import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, soyosource_modbus
from esphome.const import (
    CONF_ID,
    CONF_BATTERY_VOLTAGE,
    CONF_TEMPERATURE,
    DEVICE_CLASS_CURRENT,
    DEVICE_CLASS_EMPTY,
    DEVICE_CLASS_TEMPERATURE,
    DEVICE_CLASS_POWER,
    DEVICE_CLASS_VOLTAGE,
    ICON_EMPTY,
    ICON_CURRENT_AC,
    STATE_CLASS_MEASUREMENT,
    UNIT_AMPERE,
    UNIT_CELSIUS,
    UNIT_EMPTY,
    UNIT_HERTZ,
    UNIT_VOLT,
    UNIT_WATT,
)

AUTO_LOAD = ["soyosource_modbus"]
CODEOWNERS = ["@syssi"]

CONF_BATTERY_CURRENT = "battery_current"
CONF_BATTERY_POWER = "battery_power"
CONF_AC_VOLTAGE = "ac_voltage"
CONF_AC_FREQUENCY = "ac_frequency"
CONF_ERROR_BITS = "error_bits"

ICON_ERROR_BITS = "mdi:alert-circle-outline"

SENSORS = [
    CONF_BATTERY_VOLTAGE,
    CONF_BATTERY_CURRENT,
    CONF_BATTERY_POWER,
    CONF_AC_VOLTAGE,
    CONF_AC_FREQUENCY,
    CONF_TEMPERATURE,
    CONF_ERROR_BITS,
]

soyosource_inverter_ns = cg.esphome_ns.namespace("soyosource_inverter")
SoyosourceInverter = soyosource_inverter_ns.class_(
    "SoyosourceInverter", cg.PollingComponent, soyosource_modbus.SoyosourceModbusDevice
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SoyosourceInverter),
            cv.Optional(CONF_BATTERY_VOLTAGE): sensor.sensor_schema(
                UNIT_VOLT, ICON_EMPTY, 2, DEVICE_CLASS_VOLTAGE, STATE_CLASS_MEASUREMENT
            ),
            cv.Optional(CONF_BATTERY_CURRENT): sensor.sensor_schema(
                UNIT_AMPERE,
                ICON_EMPTY,
                2,
                DEVICE_CLASS_CURRENT,
                STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BATTERY_CURRENT): sensor.sensor_schema(
                UNIT_AMPERE,
                ICON_EMPTY,
                2,
                DEVICE_CLASS_CURRENT,
                STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_BATTERY_POWER): sensor.sensor_schema(
                UNIT_WATT, ICON_EMPTY, 2, DEVICE_CLASS_POWER, STATE_CLASS_MEASUREMENT
            ),
            cv.Optional(CONF_AC_VOLTAGE): sensor.sensor_schema(
                UNIT_VOLT, ICON_EMPTY, 1, DEVICE_CLASS_VOLTAGE, STATE_CLASS_MEASUREMENT
            ),
            cv.Optional(CONF_AC_FREQUENCY): sensor.sensor_schema(
                UNIT_HERTZ,
                ICON_CURRENT_AC,
                1,
                DEVICE_CLASS_EMPTY,
                STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_TEMPERATURE): sensor.sensor_schema(
                UNIT_CELSIUS,
                ICON_EMPTY,
                1,
                DEVICE_CLASS_TEMPERATURE,
                STATE_CLASS_MEASUREMENT,
            ),
            cv.Optional(CONF_ERROR_BITS): sensor.sensor_schema(
                UNIT_EMPTY,
                ICON_ERROR_BITS,
                0,
                DEVICE_CLASS_EMPTY,
                STATE_CLASS_MEASUREMENT,
            ),
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(soyosource_modbus.soyosource_modbus_device_schema(0x23))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await soyosource_modbus.register_soyosource_modbus_device(var, config)

    for key in SENSORS:
        if key in config:
            conf = config[key]
            sens = await sensor.new_sensor(conf)
            cg.add(getattr(var, f"set_{key}_sensor")(sens))
