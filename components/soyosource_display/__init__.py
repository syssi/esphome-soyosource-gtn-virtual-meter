import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@syssi"]

DEPENDENCIES = ["uart"]
AUTO_LOAD = ["button", "number", "select", "sensor", "text_sensor"]
MULTI_CONF = True

CONF_SOYOSOURCE_DISPLAY_ID = "soyosource_display_id"
CONF_PROTOCOL_VERSION = "protocol_version"

soyosource_display_ns = cg.esphome_ns.namespace("soyosource_display")
SoyosourceDisplay = soyosource_display_ns.class_(
    "SoyosourceDisplay", cg.PollingComponent, uart.UARTDevice
)

ProtocolVersion = soyosource_display_ns.enum("ProtocolVersion")
PROTOCOL_VERSION_OPTIONS = {
    "SOYOSOURCE_WIFI_VERSION": ProtocolVersion.SOYOSOURCE_WIFI_VERSION,
    "SOYOSOURCE_DISPLAY_VERSION": ProtocolVersion.SOYOSOURCE_DISPLAY_VERSION,
}

CONF_SOYOSOURCE_DISPLAY_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_DISPLAY_ID): cv.use_id(SoyosourceDisplay),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SoyosourceDisplay),
            cv.Optional(
                CONF_PROTOCOL_VERSION, default="SOYOSOURCE_WIFI_VERSION"
            ): cv.enum(PROTOCOL_VERSION_OPTIONS, upper=True),
        }
    )
    .extend(cv.polling_component_schema("2s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await uart.register_uart_device(var, config)

    cg.add(var.set_protocol_version(config[CONF_PROTOCOL_VERSION]))
