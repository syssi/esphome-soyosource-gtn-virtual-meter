import esphome.codegen as cg
from esphome.components import button
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_RESTART

from .. import (
    CONF_SOYOSOURCE_DISPLAY_COMPONENT_SCHEMA,
    CONF_SOYOSOURCE_DISPLAY_ID,
    soyosource_display_ns,
)

DEPENDENCIES = ["soyosource_display"]

CODEOWNERS = ["@syssi"]

# CONF_RESTART = "restart"

ICON_RESTART = "mdi:restart"

BUTTONS = {
    CONF_RESTART: 0x11,
}

SoyosourceButton = soyosource_display_ns.class_(
    "SoyosourceButton", button.Button, cg.Component
)

CONFIG_SCHEMA = CONF_SOYOSOURCE_DISPLAY_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_RESTART): button.button_schema(
            SoyosourceButton,
            icon=ICON_RESTART,
        ).extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_DISPLAY_ID])
    for key, address in BUTTONS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await button.register_button(var, conf)
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
