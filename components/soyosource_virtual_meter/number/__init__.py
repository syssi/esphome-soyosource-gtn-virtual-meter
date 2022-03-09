import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import number
from esphome.const import (
    CONF_ICON,
    CONF_ID,
    CONF_INITIAL_VALUE,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_RESTORE_VALUE,
    CONF_STEP,
    CONF_UNIT_OF_MEASUREMENT,
    UNIT_WATT,
)

from .. import (
    CONF_SOYOSOURCE_VIRTUAL_METER_ID,
    DEFAULT_MAX_POWER_DEMAND,
    DEFAULT_MIN_POWER_DEMAND,
    SoyosourceVirtualMeter,
    soyosource_virtual_meter_ns,
)

DEPENDENCIES = ["soyosource_virtual_meter"]
CODEOWNERS = ["@syssi"]

DEFAULT_STEP = 1

CONF_MANUAL_POWER_DEMAND = "manual_power_demand"
CONF_MAX_POWER_DEMAND = "max_power_demand"

ICON_MANUAL_POWER_DEMAND = "mdi:home-lightning-bolt-outline"
ICON_MAX_POWER_DEMAND = "mdi:transmission-tower-import"

NUMBERS = {
    CONF_MANUAL_POWER_DEMAND: 0x00,
    CONF_MAX_POWER_DEMAND: 0x01,
}

SoyosourceNumber = soyosource_virtual_meter_ns.class_(
    "SoyosourceNumber", number.Number, cg.Component
)


def validate_min_max(config):
    max_power_demand = cv.int_(config[CONF_MAX_VALUE])
    min_power_demand = cv.int_(config[CONF_MIN_VALUE])
    if (max_power_demand - min_power_demand) < 0:
        raise cv.Invalid(
            "Maximum power demand must be greater than minimum power demand."
        )

    return config


def validate(config):
    if CONF_INITIAL_VALUE not in config:
        config[CONF_INITIAL_VALUE] = config[CONF_MIN_VALUE]

    return config


CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_VIRTUAL_METER_ID): cv.use_id(
            SoyosourceVirtualMeter
        ),
        cv.Optional(CONF_MANUAL_POWER_DEMAND): cv.All(
            number.NUMBER_SCHEMA.extend(
                {
                    cv.GenerateID(): cv.declare_id(SoyosourceNumber),
                    cv.Optional(
                        CONF_ICON, default=ICON_MANUAL_POWER_DEMAND
                    ): number.icon,
                    cv.Optional(
                        CONF_MIN_VALUE, default=DEFAULT_MIN_POWER_DEMAND
                    ): cv.float_,
                    cv.Optional(
                        CONF_MAX_VALUE, default=DEFAULT_MAX_POWER_DEMAND
                    ): cv.float_,
                    cv.Optional(CONF_STEP, default=DEFAULT_STEP): cv.float_,
                    cv.Optional(
                        CONF_UNIT_OF_MEASUREMENT, default=UNIT_WATT
                    ): cv.string_strict,
                    cv.Optional(CONF_INITIAL_VALUE): cv.float_,
                    cv.Optional(CONF_RESTORE_VALUE, default=False): cv.boolean,
                }
            ).extend(cv.COMPONENT_SCHEMA),
            validate_min_max,
            validate,
        ),
        cv.Optional(CONF_MAX_POWER_DEMAND): cv.All(
            number.NUMBER_SCHEMA.extend(
                {
                    cv.GenerateID(): cv.declare_id(SoyosourceNumber),
                    cv.Optional(CONF_ICON, default=ICON_MAX_POWER_DEMAND): number.icon,
                    cv.Optional(
                        CONF_MIN_VALUE, default=DEFAULT_MIN_POWER_DEMAND
                    ): cv.float_,
                    cv.Optional(
                        CONF_MAX_VALUE, default=DEFAULT_MAX_POWER_DEMAND
                    ): cv.float_,
                    cv.Optional(CONF_STEP, default=DEFAULT_STEP): cv.float_,
                    cv.Optional(
                        CONF_UNIT_OF_MEASUREMENT, default=UNIT_WATT
                    ): cv.string_strict,
                    cv.Optional(CONF_INITIAL_VALUE): cv.float_,
                    cv.Optional(CONF_RESTORE_VALUE, default=False): cv.boolean,
                }
            ).extend(cv.COMPONENT_SCHEMA),
            validate_min_max,
            validate,
        ),
    }
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_SOYOSOURCE_VIRTUAL_METER_ID])
    for key, address in NUMBERS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            yield cg.register_component(var, conf)
            yield number.register_number(
                var,
                conf,
                min_value=conf[CONF_MIN_VALUE],
                max_value=conf[CONF_MAX_VALUE],
                step=conf[CONF_STEP],
            )
            cg.add(getattr(hub, f"set_{key}_number")(var))
            cg.add(var.set_parent(hub))
            cg.add(var.set_initial_value(conf[CONF_INITIAL_VALUE]))
            cg.add(var.set_restore_value(conf[CONF_RESTORE_VALUE]))
            cg.add(var.set_address(address))
