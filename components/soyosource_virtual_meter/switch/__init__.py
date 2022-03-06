import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import switch
from esphome.const import CONF_ICON, CONF_ID

from .. import (
    CONF_SOYOSOURCE_VIRTUAL_METER_ID,
    SoyosourceVirtualMeter,
    soyosource_virtual_meter_ns,
)

DEPENDENCIES = ["soyosource_virtual_meter"]

CODEOWNERS = ["@syssi"]

CONF_MANUAL_MODE = "manual_mode"

ICON_MANUAL_MODE = "mdi:auto-fix"

SWITCHES = {
    CONF_MANUAL_MODE: 0x1039,
}

SoyosourceSwitch = soyosource_virtual_meter_ns.class_(
    "SoyosourceSwitch", switch.Switch, cg.Component
)

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_VIRTUAL_METER_ID): cv.use_id(
            SoyosourceVirtualMeter
        ),
        cv.Optional(CONF_MANUAL_MODE): switch.SWITCH_SCHEMA.extend(
            {
                cv.GenerateID(): cv.declare_id(SoyosourceSwitch),
                cv.Optional(CONF_ICON, default=ICON_MANUAL_MODE): switch.icon,
            }
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_SOYOSOURCE_VIRTUAL_METER_ID])
    for key, address in SWITCHES.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            yield cg.register_component(var, conf)
            yield switch.register_switch(var, conf)
            cg.add(getattr(hub, f"set_{key}_switch")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
