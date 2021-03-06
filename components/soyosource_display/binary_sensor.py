import esphome.codegen as cg
from esphome.components import binary_sensor
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from . import CONF_SOYOSOURCE_DISPLAY_ID, SoyosourceDisplay

DEPENDENCIES = ["soyosource_display"]

CODEOWNERS = ["@syssi"]

CONF_FAN_RUNNING = "fan_running"
CONF_LIMITER_CONNECTED = "limiter_connected"

ICON_FAN_RUNNING = "mdi:fan"
ICON_LIMITER_CONNECTED = "mdi:connection"

BINARY_SENSORS = [
    CONF_FAN_RUNNING,
    CONF_LIMITER_CONNECTED,
]

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_DISPLAY_ID): cv.use_id(SoyosourceDisplay),
        cv.Optional(CONF_FAN_RUNNING): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_FAN_RUNNING): cv.icon,
            }
        ),
        cv.Optional(CONF_LIMITER_CONNECTED): binary_sensor.BINARY_SENSOR_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(binary_sensor.BinarySensor),
                cv.Optional(CONF_ICON, default=ICON_LIMITER_CONNECTED): cv.icon,
            }
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_DISPLAY_ID])
    for key in BINARY_SENSORS:
        if key in config:
            conf = config[key]
            sens = cg.new_Pvariable(conf[CONF_ID])
            await binary_sensor.register_binary_sensor(sens, conf)
            cg.add(getattr(hub, f"set_{key}_binary_sensor")(sens))
