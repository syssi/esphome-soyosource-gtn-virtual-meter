import esphome.codegen as cg
from esphome.components import text_sensor
import esphome.config_validation as cv
from esphome.const import (
    CONF_ENTITY_CATEGORY,
    CONF_ICON,
    CONF_ID,
    ENTITY_CATEGORY_DIAGNOSTIC,
)

from . import CONF_SOYOSOURCE_DISPLAY_ID, SoyosourceDisplay
from .const import CONF_OPERATION_MODE

DEPENDENCIES = ["soyosource_display"]

CODEOWNERS = ["@syssi"]

CONF_ERRORS = "errors"
# CONF_OPERATION_MODE from const
CONF_OPERATION_STATUS = "operation_status"

ICON_ERRORS = "mdi:alert-circle-outline"
ICON_OPERATION_MODE = "mdi:heart-pulse"
ICON_OPERATION_STATUS = "mdi:heart-pulse"

TEXT_SENSORS = [
    CONF_ERRORS,
    CONF_OPERATION_MODE,
    CONF_OPERATION_STATUS,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_DISPLAY_ID): cv.use_id(SoyosourceDisplay),
        cv.Optional(CONF_ERRORS): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_ERRORS): cv.icon,
                cv.Optional(
                    CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_DIAGNOSTIC
                ): cv.entity_category,
            }
        ),
        cv.Optional(CONF_OPERATION_MODE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_OPERATION_MODE): cv.icon,
            }
        ),
        cv.Optional(CONF_OPERATION_STATUS): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_OPERATION_STATUS): cv.icon,
            }
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_DISPLAY_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
