import esphome.codegen as cg
from esphome.components import soyosource_modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

AUTO_LOAD = ["soyosource_modbus", "binary_sensor", "sensor", "text_sensor"]
CODEOWNERS = ["@syssi"]
MULTI_CONF = True

CONF_SOYOSOURCE_INVERTER_ID = "soyosource_inverter_id"

soyosource_inverter_ns = cg.esphome_ns.namespace("soyosource_inverter")
SoyosourceInverter = soyosource_inverter_ns.class_(
    "SoyosourceInverter", cg.PollingComponent, soyosource_modbus.SoyosourceModbusDevice
)

CONF_SOYOSOURCE_INVERTER_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_INVERTER_ID): cv.use_id(SoyosourceInverter),
    }
)

CONFIG_SCHEMA = (
    cv.Schema({cv.GenerateID(): cv.declare_id(SoyosourceInverter)})
    .extend(cv.polling_component_schema("5s"))
    .extend(soyosource_modbus.soyosource_modbus_device_schema(0x23))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await soyosource_modbus.register_soyosource_modbus_device(var, config)
