import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ID

from . import CONF_SOYOSOURCE_INVERTER_ID, SOYOSOURCE_INVERTER_COMPONENT_SCHEMA

DEPENDENCIES = ["soyosource_inverter"]

CODEOWNERS = ["@syssi"]

CONF_OPERATION_STATUS = "operation_status"

ICON_OPERATION_STATUS = "mdi:heart-pulse"

TEXT_SENSORS = [
    CONF_OPERATION_STATUS,
]

CONFIG_SCHEMA = SOYOSOURCE_INVERTER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_OPERATION_STATUS): text_sensor.text_sensor_schema(
            text_sensor.TextSensor, icon=ICON_OPERATION_STATUS
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_INVERTER_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = await text_sensor.new_text_sensor(conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
