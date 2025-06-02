import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    CONF_ID,
    CONF_INITIAL_VALUE,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_RESTORE_VALUE,
    CONF_STEP,
    UNIT_EMPTY,
    UNIT_WATT,
    UNIT_MILLISECOND,
)

from .. import (
    CONF_BUFFER,
    CONF_POWER_DEMAND_DIVIDER,
    CONF_SOYOSOURCE_VIRTUAL_METER_ID,
    CONF_MAX_POWER_SENSOR_LATENCY_MS,
    DEFAULT_BUFFER,
    DEFAULT_MAX_BUFFER,
    DEFAULT_MAX_POWER_DEMAND,
    DEFAULT_MAX_POWER_DEMAND_DIVIDER,
    DEFAULT_MIN_BUFFER,
    DEFAULT_MIN_POWER_DEMAND,
    DEFAULT_MIN_POWER_DEMAND_DIVIDER,
    DEFAULT_POWER_DEMAND_DIVIDER,
    DEFAULT_MAX_POWER_SENSOR_LATENCY_MS,
    DEFAULT_MIN_MAX_POWER_SENSOR_LATENCY_MS,
    DEFAULT_MAX_MAX_POWER_SENSOR_LATENCY_MS,
    SoyosourceVirtualMeter,
    soyosource_virtual_meter_ns,
)

DEPENDENCIES = ["soyosource_virtual_meter"]
CODEOWNERS = ["@syssi"]

DEFAULT_STEP = 1

CONF_MANUAL_POWER_DEMAND = "manual_power_demand"
CONF_MAX_POWER_DEMAND = "max_power_demand"

ICON_BUFFER = "mdi:vector-difference"
ICON_MANUAL_POWER_DEMAND = "mdi:home-lightning-bolt-outline"
ICON_MAX_POWER_DEMAND = "mdi:transmission-tower-import"
ICON_POWER_DEMAND_DIVIDER = "mdi:chart-arc"
ICON_POWER_SENSOR_LATENCY = "mdi:timer-sync-outline"

NUMBERS = {
    CONF_MANUAL_POWER_DEMAND: 0x00,
    CONF_MAX_POWER_DEMAND: 0x01,
    CONF_BUFFER: 0x02,
    CONF_POWER_DEMAND_DIVIDER: 0x03,
    CONF_MAX_POWER_SENSOR_LATENCY_MS: 0x04,
}

SoyosourceNumber = soyosource_virtual_meter_ns.class_(
    "SoyosourceNumber", number.Number, cg.Component
)


def validate_min_max(config):
    max = cv.int_(config[CONF_MAX_VALUE])
    min = cv.int_(config[CONF_MIN_VALUE])
    if (max - min) < 0:
        raise cv.Invalid("Upper limit must be greater than the lower limit.")

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
        cv.Optional(CONF_BUFFER): cv.All(
            number.number_schema(
                SoyosourceNumber,
                icon=ICON_BUFFER,
                unit_of_measurement=UNIT_WATT,
            )
            .extend(
                {
                    cv.Optional(CONF_MIN_VALUE, default=DEFAULT_MIN_BUFFER): cv.float_,
                    cv.Optional(CONF_MAX_VALUE, default=DEFAULT_MAX_BUFFER): cv.float_,
                    cv.Optional(CONF_STEP, default=DEFAULT_STEP): cv.float_,
                    cv.Optional(CONF_INITIAL_VALUE, default=DEFAULT_BUFFER): cv.float_,
                    cv.Optional(CONF_RESTORE_VALUE, default=False): cv.boolean,
                }
            )
            .extend(cv.COMPONENT_SCHEMA),
            validate_min_max,
            validate,
        ),
        cv.Optional(CONF_MANUAL_POWER_DEMAND): cv.All(
            number.number_schema(
                SoyosourceNumber,
                icon=ICON_MANUAL_POWER_DEMAND,
                unit_of_measurement=UNIT_WATT,
            )
            .extend(
                {
                    cv.Optional(
                        CONF_MIN_VALUE, default=DEFAULT_MIN_POWER_DEMAND
                    ): cv.float_,
                    cv.Optional(
                        CONF_MAX_VALUE, default=DEFAULT_MAX_POWER_DEMAND
                    ): cv.float_,
                    cv.Optional(CONF_STEP, default=DEFAULT_STEP): cv.float_,
                    cv.Optional(CONF_INITIAL_VALUE): cv.float_,
                    cv.Optional(CONF_RESTORE_VALUE, default=False): cv.boolean,
                }
            )
            .extend(cv.COMPONENT_SCHEMA),
            validate_min_max,
            validate,
        ),
        cv.Optional(CONF_MAX_POWER_DEMAND): cv.All(
            number.number_schema(
                SoyosourceNumber,
                icon=ICON_MAX_POWER_DEMAND,
                unit_of_measurement=UNIT_WATT,
            )
            .extend(
                {
                    cv.Optional(
                        CONF_MIN_VALUE, default=DEFAULT_MIN_POWER_DEMAND
                    ): cv.float_,
                    cv.Optional(
                        CONF_MAX_VALUE, default=DEFAULT_MAX_POWER_DEMAND
                    ): cv.float_,
                    cv.Optional(CONF_STEP, default=DEFAULT_STEP): cv.float_,
                    cv.Optional(CONF_INITIAL_VALUE): cv.float_,
                    cv.Optional(CONF_RESTORE_VALUE, default=False): cv.boolean,
                }
            )
            .extend(cv.COMPONENT_SCHEMA),
            validate_min_max,
            validate,
        ),
        cv.Optional(CONF_POWER_DEMAND_DIVIDER): cv.All(
            number.number_schema(
                SoyosourceNumber,
                icon=ICON_POWER_DEMAND_DIVIDER,
                unit_of_measurement=UNIT_EMPTY,
            )
            .extend(
                {
                    cv.Optional(
                        CONF_MIN_VALUE, default=DEFAULT_MIN_POWER_DEMAND_DIVIDER
                    ): cv.float_,
                    cv.Optional(
                        CONF_MAX_VALUE, default=DEFAULT_MAX_POWER_DEMAND_DIVIDER
                    ): cv.float_,
                    cv.Optional(CONF_STEP, default=DEFAULT_STEP): cv.float_,
                    cv.Optional(
                        CONF_INITIAL_VALUE, default=DEFAULT_POWER_DEMAND_DIVIDER
                    ): cv.float_,
                    cv.Optional(CONF_RESTORE_VALUE, default=False): cv.boolean,
                }
            )
            .extend(cv.COMPONENT_SCHEMA),
            validate_min_max,
            validate,
        ),
        cv.Optional(CONF_MAX_POWER_SENSOR_LATENCY_MS): cv.All(
            number.NUMBER_SCHEMA.extend(
                {
                    cv.GenerateID(): cv.declare_id(SoyosourceNumber),
                    cv.Optional(CONF_ICON, default=ICON_POWER_SENSOR_LATENCY): cv.icon,
                    cv.Optional(
                        CONF_MIN_VALUE, default=DEFAULT_MIN_MAX_POWER_SENSOR_LATENCY_MS
                    ): cv.float_,
                    cv.Optional(
                        CONF_MAX_VALUE, default=DEFAULT_MAX_MAX_POWER_SENSOR_LATENCY_MS
                    ): cv.float_,
                    cv.Optional(CONF_STEP, default=DEFAULT_STEP): cv.float_,
                    cv.Optional(
                        CONF_UNIT_OF_MEASUREMENT, default=UNIT_MILLISECOND
                    ): cv.string_strict,
                    cv.Optional(
                        CONF_INITIAL_VALUE, default=DEFAULT_MAX_POWER_SENSOR_LATENCY_MS
                    ): cv.float_,
                    cv.Optional(CONF_RESTORE_VALUE, default=False): cv.boolean,
                }
            ).extend(cv.COMPONENT_SCHEMA),
            validate_min_max,
            validate,
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_VIRTUAL_METER_ID])
    for key, address in NUMBERS.items():
        if key in config:
            conf = config[key]
            var = cg.new_Pvariable(conf[CONF_ID])
            await cg.register_component(var, conf)
            await number.register_number(
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
