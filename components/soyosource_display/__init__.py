import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

AUTO_LOAD = ["button", "number", "select", "sensor", "text_sensor"]

soyosource_display_ns = cg.esphome_ns.namespace("soyosource_display")
SoyosourceDisplay = soyosource_display_ns.class_(
    "SoyosourceDisplay", cg.PollingComponent, uart.UARTDevice
)

MULTI_CONF = True

CONF_SOYOSOURCE_DISPLAY_ID = "soyosource_display_id"

CONF_SOYOSOURCE_DISPLAY_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_DISPLAY_ID): cv.use_id(SoyosourceDisplay),
    }
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SoyosourceDisplay),
        }
    )
    .extend(cv.polling_component_schema("2s"))
    .extend(uart.UART_DEVICE_SCHEMA)
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    await uart.register_uart_device(var, config)
