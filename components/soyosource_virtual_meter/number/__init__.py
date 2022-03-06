import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    CONF_ICON,
    CONF_ID,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_MULTIPLY,
    CONF_STEP,
)

from .. import (
    CONF_SOYOSOURCE_VIRTUAL_METER_ID,
    DEFAULT_MIN_POWER_DEMAND,
    DEFAULT_MAX_POWER_DEMAND,
    SoyosourceVirtualMeter,
    soyosource_virtual_meter_ns,
)

DEPENDENCIES = ["soyosource_virtual_meter"]
CODEOWNERS = ["@syssi"]

DEFAULT_STEP = 1

CONF_MANUAL_POWER_DEMAND = "manual_power_demand"

ICON_MANUAL_POWER_DEMAND = "mdi:home-lightning-bolt-outline"

NUMBERS = [
    CONF_MANUAL_POWER_DEMAND,
]

SoyosourceNumber = soyosource_virtual_meter_ns.class_(
    "SoyosourceNumber", number.Number, cg.Component
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_VIRTUAL_METER_ID): cv.use_id(
            SoyosourceVirtualMeter
        ),
        cv.Optional(CONF_MANUAL_POWER_DEMAND): number.NUMBER_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(SoyosourceNumber),
                cv.Optional(CONF_ICON, default=ICON_MANUAL_POWER_DEMAND): number.icon,
                cv.Optional(CONF_MIN_VALUE, default=DEFAULT_MIN_POWER_DEMAND): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=DEFAULT_MAX_POWER_DEMAND): cv.float_,
                cv.Optional(CONF_STEP, default=DEFAULT_STEP): cv.float_,
            }
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_SOYOSOURCE_VIRTUAL_METER_ID])
    for key in NUMBERS:
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            yield cg.register_component(var, conf)
            yield number.register_number(var, conf)
            cg.add(getattr(hub, f"set_{key}_number")(var))
            cg.add(var.set_parent(hub))
