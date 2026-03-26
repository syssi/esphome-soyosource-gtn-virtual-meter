import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import ENTITY_CATEGORY_DIAGNOSTIC

from . import CONF_SOYOSOURCE_DISPLAY_ID, SOYOSOURCE_DISPLAY_COMPONENT_SCHEMA

DEPENDENCIES = ["soyosource_display"]

CODEOWNERS = ["@syssi"]

CONF_FAN_RUNNING = "fan_running"
CONF_LIMITER_CONNECTED = "limiter_connected"

BINARY_SENSORS = [
    CONF_FAN_RUNNING,
    CONF_LIMITER_CONNECTED,
]

CONFIG_SCHEMA = SOYOSOURCE_DISPLAY_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_FAN_RUNNING): binary_sensor.binary_sensor_schema(
            icon="mdi:fan",
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
        cv.Optional(CONF_LIMITER_CONNECTED): binary_sensor.binary_sensor_schema(
            icon="mdi:connection",
            entity_category=ENTITY_CATEGORY_DIAGNOSTIC,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_DISPLAY_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = await binary_sensor.new_binary_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
