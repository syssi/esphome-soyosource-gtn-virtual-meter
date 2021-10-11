import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import text_sensor
from esphome.const import CONF_ID, CONF_ICON
from . import SoyosourceInverter, CONF_SOYOSOURCE_INVERTER_ID

DEPENDENCIES = ["soyosource_inverter"]

CODEOWNERS = ["@syssi"]

CONF_OPERATION_MODE = "operation_mode"

ICON_OPERATION_MODE = "mdi:heart-pulse"

TEXT_SENSORS = [
    CONF_OPERATION_MODE,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_INVERTER_ID): cv.use_id(SoyosourceInverter),
        cv.Optional(CONF_OPERATION_MODE): text_sensor.TEXT_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(text_sensor.TextSensor),
                cv.Optional(CONF_ICON, default=ICON_OPERATION_MODE): cv.icon,
            }
        ),
    }
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_SOYOSOURCE_INVERTER_ID])
    for key in TEXT_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            yield text_sensor.register_text_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_text_sensor")(sens))
