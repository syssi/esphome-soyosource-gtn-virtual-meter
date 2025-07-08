import esphome.codegen as cg
from esphome.components import switch
import esphome.config_validation as cv
from esphome.const import CONF_ID, CONF_RESTORE_MODE

from .. import (
    CONF_SOYOSOURCE_VIRTUAL_METER_COMPONENT_SCHEMA,
    CONF_SOYOSOURCE_VIRTUAL_METER_ID,
    soyosource_virtual_meter_ns,
)

DEPENDENCIES = ["soyosource_virtual_meter"]

CODEOWNERS = ["@syssi"]

CONF_MANUAL_MODE = "manual_mode"
CONF_EMERGENCY_POWER_OFF = "emergency_power_off"

ICON_MANUAL_MODE = "mdi:auto-fix"
ICON_EMERGENCY_POWER_OFF = "mdi:power"

SWITCHES = [
    CONF_MANUAL_MODE,
    CONF_EMERGENCY_POWER_OFF,
]

SoyosourceSwitch = soyosource_virtual_meter_ns.class_(
    "SoyosourceSwitch", switch.Switch, cg.Component
)
SoyosourceSwitchRestoreMode = soyosource_virtual_meter_ns.enum(
    "SoyosourceSwitchRestoreMode"
)

RESTORE_MODES = {
    "RESTORE_DEFAULT_OFF": SoyosourceSwitchRestoreMode.SOYOSOURCE_SWITCH_RESTORE_DEFAULT_OFF,
    "RESTORE_DEFAULT_ON": SoyosourceSwitchRestoreMode.SOYOSOURCE_SWITCH_RESTORE_DEFAULT_ON,
    "ALWAYS_OFF": SoyosourceSwitchRestoreMode.SOYOSOURCE_SWITCH_ALWAYS_OFF,
    "ALWAYS_ON": SoyosourceSwitchRestoreMode.SOYOSOURCE_SWITCH_ALWAYS_ON,
}

CONFIG_SCHEMA = CONF_SOYOSOURCE_VIRTUAL_METER_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_MANUAL_MODE): switch.switch_schema(
            SoyosourceSwitch,
            icon=ICON_MANUAL_MODE,
        )
        .extend(
            {
                cv.Optional(CONF_RESTORE_MODE, default="RESTORE_DEFAULT_OFF"): cv.enum(
                    RESTORE_MODES, upper=True, space="_"
                ),
            }
        )
        .extend(cv.COMPONENT_SCHEMA),
        cv.Optional(CONF_EMERGENCY_POWER_OFF): switch.switch_schema(
            SoyosourceSwitch,
            icon=ICON_EMERGENCY_POWER_OFF,
        )
        .extend(
            {
                cv.Optional(CONF_RESTORE_MODE, default="RESTORE_DEFAULT_OFF"): cv.enum(
                    RESTORE_MODES, upper=True, space="_"
                ),
            }
        )
        .extend(cv.COMPONENT_SCHEMA),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_VIRTUAL_METER_ID])
    for key in SWITCHES:
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await switch.register_switch(var, conf)
            cg.add(getattr(hub, f"set_{key}_switch")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_restore_mode(conf[CONF_RESTORE_MODE]))
