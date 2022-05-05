import esphome.codegen as cg
from esphome.components import uart
import esphome.config_validation as cv
from esphome.const import CONF_ID

DEPENDENCIES = ["uart"]

soyosource_inverter_emulator_ns = cg.esphome_ns.namespace(
    "soyosource_inverter_emulator"
)
SoyosourceInverterEmulator = soyosource_inverter_emulator_ns.class_(
    "SoyosourceInverterEmulator", cg.Component, uart.UARTDevice
)

MULTI_CONF = True

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(SoyosourceInverterEmulator),
    }
).extend(uart.UART_DEVICE_SCHEMA)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    await uart.register_uart_device(var, config)
