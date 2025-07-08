import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import (
    CONF_SOYOSOURCE_VIRTUAL_METER_ID,
    SOYOSOURCE_VIRTUAL_METER_COMPONENT_SCHEMA,
)

DEPENDENCIES = ["soyosource_virtual_meter"]

CODEOWNERS = ["@syssi"]

CONF_OPERATION_MODE = "operation_mode"

ICON_OPERATION_MODE = "mdi:heart-pulse"

TEXT_SENSORS = [
    CONF_OPERATION_MODE,
]

CONFIG_SCHEMA = SOYOSOURCE_VIRTUAL_METER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_OPERATION_MODE): text_sensor.text_sensor_schema(
            text_sensor.TextSensor,
            icon=ICON_OPERATION_MODE,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_VIRTUAL_METER_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
