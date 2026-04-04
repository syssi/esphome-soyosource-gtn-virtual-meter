import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_SOYOSOURCE_INVERTER_ID, SOYOSOURCE_INVERTER_COMPONENT_SCHEMA

DEPENDENCIES = ["soyosource_inverter"]

CODEOWNERS = ["@syssi"]

CONF_FAN_RUNNING = "fan_running"

BINARY_SENSOR_DEFS = {
    CONF_FAN_RUNNING: {
        "icon": "mdi:fan",
        "entity_category": ENTITY_CATEGORY_DIAGNOSTIC,
    },
}

CONFIG_SCHEMA = SOYOSOURCE_INVERTER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(key): binary_sensor.binary_sensor_schema(**kwargs)
        for key, kwargs in BINARY_SENSOR_DEFS.items()
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_INVERTER_ID])
    for key in BINARY_SENSOR_DEFS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
