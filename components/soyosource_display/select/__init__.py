import esphome.codegen as cg
from esphome.components import select
import esphome.config_validation as cv
from esphome.const import CONF_ICON, CONF_ID

from .. import (
    CONF_SOYOSOURCE_DISPLAY_COMPONENT_SCHEMA,
    CONF_SOYOSOURCE_DISPLAY_ID,
    soyosource_display_ns,
)

from ..const import CONF_OPERATION_MODE

DEPENDENCIES = ["soyosource_display"]

CODEOWNERS = ["@syssi"]

# CONF_OPERATION_MODE from const
CONF_OPTIONSMAP = "optionsmap"

ICON_OPERATION_MODE = "mdi:heart-pulse"

SoyosourceSelect = soyosource_display_ns.class_(
    "SoyosourceSelect", select.Select, cg.Component
)


def ensure_option_map(value):
    cv.check_not_templatable(value)
    option = cv.All(cv.int_range(0, 2**16 - 1))
    mapping = cv.All(cv.string_strict)
    options_map_schema = cv.Schema({option: mapping})
    value = options_map_schema(value)

    all_values = list(value.keys())
    unique_values = set(value.keys())
    if len(all_values) != len(unique_values):
        raise cv.Invalid("Mapping values must be unique.")

    return value


SELECTS = {
    CONF_OPERATION_MODE: 0x0A,
}

SOYOSOURCE_SELECT_SCHEMA = select.SELECT_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(SoyosourceSelect),
        cv.Optional(CONF_ICON, default=ICON_OPERATION_MODE): select.icon,
        cv.Optional(CONF_OPTIONSMAP): ensure_option_map,
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = CONF_SOYOSOURCE_DISPLAY_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_OPERATION_MODE): SOYOSOURCE_SELECT_SCHEMA,
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_DISPLAY_ID])

    for key, address in SELECTS.items():
        if key in config:
            conf = config[key]
            options_map = conf[CONF_OPTIONSMAP]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await select.register_select(var, conf, options=list(options_map.values()))
            cg.add(var.set_select_mappings(list(options_map.keys())))

            cg.add(getattr(hub, f"set_{key}_select")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_holding_register(address))
