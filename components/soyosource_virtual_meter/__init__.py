import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor, soyosource_modbus
from esphome.const import CONF_ID

AUTO_LOAD = ["soyosource_modbus", "sensor", "switch"]
CODEOWNERS = ["@syssi"]
MULTI_CONF = True

CONF_SOYOSOURCE_VIRTUAL_METER_ID = "soyosource_virtual_meter_id"
CONF_POWER_ID = "power_id"
CONF_MIN_POWER_DEMAND = "min_power_demand"
CONF_MAX_POWER_DEMAND = "max_power_demand"
CONF_BUFFER = "buffer"

DEFAULT_BUFFER = 0
DEFAULT_MIN_POWER_DEMAND = 0
DEFAULT_MAX_POWER_DEMAND = 900

soyosource_virtual_meter_ns = cg.esphome_ns.namespace("soyosource_virtual_meter")
SoyosourceVirtualMeter = soyosource_virtual_meter_ns.class_(
    "SoyosourceVirtualMeter",
    cg.PollingComponent,
    soyosource_modbus.SoyosourceModbusDevice,
)


def validate_min_max(config):
    max_power_demand = cv.int_(config[CONF_MAX_POWER_DEMAND])
    min_power_demand = cv.int_(config[CONF_MIN_POWER_DEMAND])
    if (max_power_demand - min_power_demand) < 0:
        raise cv.Invalid(
            "Maximum power demand must be greater than minimum power demand."
        )

    return config


CONFIG_SCHEMA = cv.All(
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(SoyosourceVirtualMeter),
            cv.Required(CONF_POWER_ID): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_BUFFER, default=DEFAULT_BUFFER): cv.int_range(
                min=-200, max=200
            ),
            cv.Optional(
                CONF_MIN_POWER_DEMAND, default=DEFAULT_MIN_POWER_DEMAND
            ): cv.int_range(min=0, max=2000),
            cv.Optional(
                CONF_MAX_POWER_DEMAND, default=DEFAULT_MAX_POWER_DEMAND
            ): cv.int_range(min=1, max=2000),
        }
    )
    .extend(soyosource_modbus.soyosource_modbus_device_schema(0x24))
    .extend(cv.COMPONENT_SCHEMA),
    validate_min_max,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await soyosource_modbus.register_soyosource_modbus_device(var, config)

    sens = await cg.get_variable(config[CONF_POWER_ID])
    cg.add(var.set_power_sensor(sens))
    cg.add(var.set_buffer(config[CONF_BUFFER]))
    cg.add(var.set_min_power_demand(config[CONF_MIN_POWER_DEMAND]))
    cg.add(var.set_max_power_demand(config[CONF_MAX_POWER_DEMAND]))
