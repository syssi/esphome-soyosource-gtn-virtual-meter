import esphome.codegen as cg
from esphome.components import number
import esphome.config_validation as cv
from esphome.const import (
    CONF_ENTITY_CATEGORY,
    CONF_ICON,
    CONF_ID,
    CONF_MAX_VALUE,
    CONF_MIN_VALUE,
    CONF_MODE,
    CONF_STEP,
    CONF_UNIT_OF_MEASUREMENT,
    ENTITY_CATEGORY_CONFIG,
    ICON_EMPTY,
    UNIT_SECOND,
    UNIT_VOLT,
    UNIT_WATT,
)

from .. import (
    CONF_SOYOSOURCE_DISPLAY_COMPONENT_SCHEMA,
    CONF_SOYOSOURCE_DISPLAY_ID,
    soyosource_display_ns,
)

DEPENDENCIES = ["soyosource_display"]

CODEOWNERS = ["@syssi"]

DEFAULT_STEP = 1

CONF_START_VOLTAGE = "start_voltage"
CONF_SHUTDOWN_VOLTAGE = "shutdown_voltage"
CONF_OUTPUT_POWER_LIMIT = "output_power_limit"
CONF_START_DELAY = "start_delay"

# Write settings frame: 0x55 0x0B 0x30 0x2D 0x5A 0x64 0x00 0x00 0x00 0x06 0x01 0xD2
#                         0    1    2    3    4    5    6    7    8    9   10   11
#                         |    |    |    |    |    |    |    |    |    |    |    CRC
#                         |    |    |    |    |    |    |    |    |    |    Operation mode
#                         |    |    |    |    |    |    |    |    |    Start delay
#                         |    |    |    |    |    |    |    |    Unused
#                         |    |    |    |    |    |    |    Unused
#                         |    |    |    |    |    |    Unused
#                         |    |    |    |    |    Grid frequency
#                         |    |    |    |    Output power
#                         |    |    |    Shutdown voltage
#                         |    |    Start voltage
#                         |    Write settings
#                         SOF Request

# Settings frame: 0xA6 0x00 0x00 0x63 0x02 0xD4 0x30 0x30 0x2D 0x00 0xFA 0x64 0x5A 0x06 0x7B
#                   0    1    2    3    4    5    6    7    8    9   10   11   12   13   14
#                                  |    |    |    |    |    |    |    |    |    |    |    CRC
#                                  |    |    |    |    |    |    |    |    |    |    Start delay
#                                  |    |    |    |    |    |    |    |    |    Output power
#                                  |    |    |    |    |    |    |    |    Grid frequency
#                                  |    |    |    |    |    |    Grid voltage (2 bytes)
#                                  |    |    |    |    |    Shutdown voltage
#                                  |    |    |    |    Start voltage
#                                  |    |    |    Device type
#                                  |    |    Device model
#                                  |    Operation status bitmask
#                                  Operation mode (High nibble), Frame function (Low nibble)
NUMBERS = {
    CONF_START_VOLTAGE: 0x02,
    CONF_SHUTDOWN_VOLTAGE: 0x03,
    CONF_OUTPUT_POWER_LIMIT: 0x04,
    CONF_START_DELAY: 0x09,
}

SoyosourceNumber = soyosource_display_ns.class_(
    "SoyosourceNumber", number.Number, cg.Component
)

SOYOSOURCE_NUMBER_SCHEMA = number.NUMBER_SCHEMA.extend(
    {
        cv.GenerateID(): cv.declare_id(SoyosourceNumber),
        cv.Optional(CONF_ICON, default=ICON_EMPTY): cv.icon,
        cv.Optional(CONF_STEP, default=0.01): cv.float_,
        cv.Optional(CONF_UNIT_OF_MEASUREMENT, default=UNIT_VOLT): cv.string_strict,
        cv.Optional(CONF_MODE, default="BOX"): cv.enum(number.NUMBER_MODES, upper=True),
        cv.Optional(
            CONF_ENTITY_CATEGORY, default=ENTITY_CATEGORY_CONFIG
        ): cv.entity_category,
    }
).extend(cv.COMPONENT_SCHEMA)

CONFIG_SCHEMA = CONF_SOYOSOURCE_DISPLAY_COMPONENT_SCHEMA.extend(
    {
        cv.Optional(CONF_START_VOLTAGE): SOYOSOURCE_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=90): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_SHUTDOWN_VOLTAGE): SOYOSOURCE_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=90): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
            }
        ),
        cv.Optional(CONF_OUTPUT_POWER_LIMIT): SOYOSOURCE_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=10): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=900): cv.float_,
                cv.Optional(CONF_STEP, default=10): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_WATT
                ): cv.string_strict,
            }
        ),
        cv.Optional(CONF_START_DELAY): SOYOSOURCE_NUMBER_SCHEMA.extend(
            {
                cv.Optional(CONF_MIN_VALUE, default=1): cv.float_,
                cv.Optional(CONF_MAX_VALUE, default=60): cv.float_,
                cv.Optional(CONF_STEP, default=1): cv.float_,
                cv.Optional(
                    CONF_UNIT_OF_MEASUREMENT, default=UNIT_SECOND
                ): cv.string_strict,
            }
        ),
    }
)


async def to_code(config):
    hub = await cg.get_variable(config[CONF_SOYOSOURCE_DISPLAY_ID])
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
            cg.add(var.set_holding_register(address))
