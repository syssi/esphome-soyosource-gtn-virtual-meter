import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@syssi"]

DEPENDENCIES = ["uart"]
MULTI_CONF = True

CONF_PROTOCOL_VERSION = "protocol_version"

soyosource_inverter_emulator_ns = cg.esphome_ns.namespace(
    "soyosource_inverter_emulator"
)
SoyosourceInverterEmulator = soyosource_inverter_emulator_ns.class_(
    "SoyosourceInverterEmulator", cg.Component, uart.UARTDevice
)

ProtocolVersion = soyosource_inverter_emulator_ns.enum("ProtocolVersion")
PROTOCOL_VERSION_OPTIONS = {
    "SOYOSOURCE_WIFI_VERSION": ProtocolVersion.SOYOSOURCE_WIFI_VERSION,
    "SOYOSOURCE_DISPLAY_VERSION": ProtocolVersion.SOYOSOURCE_DISPLAY_VERSION,
}

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SoyosourceInverterEmulator),
        cv.Optional(CONF_PROTOCOL_VERSION, default="SOYOSOURCE_WIFI_VERSION"): cv.enum(
            PROTOCOL_VERSION_OPTIONS, upper=True
        ),
    }
).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    await uart.register_uart_device(var, config)

    cg.add(var.set_protocol_version(config[CONF_PROTOCOL_VERSION]))
