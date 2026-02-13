import esphome.codegen as cg
from esphome.components import sensor, soyosource_modbus
import esphome.config_validation as cv
from esphome.const import CONF_ID

CODEOWNERS = ["@syssi"]

DEPENDENCIES = ["soyosource_modbus"]
AUTO_LOAD = ["number", "sensor", "switch", "text_sensor"]
MULTI_CONF = True

CONF_SOYOSOURCE_VIRTUAL_METER_ID = "soyosource_virtual_meter_id"
CONF_POWER_ID = "power_id"
CONF_POWER_SENSOR_INACTIVITY_TIMEOUT = "power_sensor_inactivity_timeout"
CONF_POWER_DEMAND_CALCULATION = "power_demand_calculation"
CONF_MIN_POWER_DEMAND = "min_power_demand"
CONF_MAX_POWER_DEMAND = "max_power_demand"
CONF_BUFFER = "buffer"
CONF_POWER_DEMAND_DIVIDER = "power_demand_divider"
CONF_OPERATION_STATUS_ID = "operation_status_id"
CONF_ZERO_OUTPUT_ON_MIN_POWER_DEMAND = "zero_output_on_min_power_demand"
CONF_MAX_POWER_SENSOR_LATENCY_MS = "max_power_sensor_latency_ms"

DEFAULT_MIN_BUFFER = -200
DEFAULT_MAX_BUFFER = 200
DEFAULT_BUFFER = 0

DEFAULT_MIN_POWER_DEMAND = 0
DEFAULT_MAX_POWER_DEMAND = 900

DEFAULT_POWER_DEMAND_DIVIDER = 1
DEFAULT_MIN_POWER_DEMAND_DIVIDER = 1
DEFAULT_MAX_POWER_DEMAND_DIVIDER = 6

DEFAULT_MAX_POWER_SENSOR_LATENCY_MS = 5000
DEFAULT_MIN_MAX_POWER_SENSOR_LATENCY_MS = 0
DEFAULT_MAX_MAX_POWER_SENSOR_LATENCY_MS = 30000

soyosource_virtual_meter_ns = cg.esphome_ns.namespace("soyosource_virtual_meter")
SoyosourceVirtualMeter = soyosource_virtual_meter_ns.class_(
    "SoyosourceVirtualMeter",
    cg.PollingComponent,
    soyosource_modbus.SoyosourceModbusDevice,
)

PowerDemandCalculation = soyosource_virtual_meter_ns.enum("PowerDemandCalculation")
POWER_DEMAND_CALCULATION_OPTIONS = {
    "DUMB_OEM_BEHAVIOR": PowerDemandCalculation.POWER_DEMAND_CALCULATION_DUMB_OEM_BEHAVIOR,
    "NEGATIVE_MEASUREMENTS_REQUIRED": PowerDemandCalculation.POWER_DEMAND_CALCULATION_NEGATIVE_MEASUREMENTS_REQUIRED,
    "RESTART_ON_CROSSING_ZERO": PowerDemandCalculation.POWER_DEMAND_CALCULATION_RESTART_ON_CROSSING_ZERO,
}

SOYOSOURCE_VIRTUAL_METER_COMPONENT_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_VIRTUAL_METER_ID): cv.use_id(
            SoyosourceVirtualMeter
        ),
    }
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
            cv.Optional(
                CONF_POWER_DEMAND_CALCULATION, default="DUMB_OEM_BEHAVIOR"
            ): cv.enum(POWER_DEMAND_CALCULATION_OPTIONS, upper=True),
            cv.Required(CONF_POWER_ID): cv.use_id(sensor.Sensor),
            cv.Optional(CONF_OPERATION_STATUS_ID): cv.use_id(sensor.Sensor),
            cv.Optional(
                CONF_POWER_SENSOR_INACTIVITY_TIMEOUT, default="20s"
            ): cv.positive_time_period_seconds,
            cv.Optional(CONF_BUFFER, default=DEFAULT_BUFFER): cv.int_range(
                min=DEFAULT_MIN_BUFFER, max=DEFAULT_MAX_BUFFER
            ),
            cv.Optional(
                CONF_POWER_DEMAND_DIVIDER, default=DEFAULT_POWER_DEMAND_DIVIDER
            ): cv.int_range(
                min=DEFAULT_MIN_POWER_DEMAND_DIVIDER,
                max=DEFAULT_MAX_POWER_DEMAND_DIVIDER,
            ),
            cv.Optional(
                CONF_MIN_POWER_DEMAND, default=DEFAULT_MIN_POWER_DEMAND
            ): cv.int_range(min=0, max=5400),
            cv.Optional(
                CONF_MAX_POWER_DEMAND, default=DEFAULT_MAX_POWER_DEMAND
            ): cv.int_range(min=1, max=5400),
            cv.Optional(CONF_ZERO_OUTPUT_ON_MIN_POWER_DEMAND, default=True): cv.boolean,
            cv.Optional(
                CONF_MAX_POWER_SENSOR_LATENCY_MS,
                default=DEFAULT_MAX_POWER_SENSOR_LATENCY_MS,
            ): cv.int_range(
                min=DEFAULT_MIN_MAX_POWER_SENSOR_LATENCY_MS,
                max=DEFAULT_MAX_MAX_POWER_SENSOR_LATENCY_MS,
            ),
        }
    )
    .extend(soyosource_modbus.soyosource_modbus_device_schema(0x24))
    .extend(cv.polling_component_schema("3s")),
    validate_min_max,
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await soyosource_modbus.register_soyosource_modbus_device(var, config)

    power_sensor = await cg.get_variable(config[CONF_POWER_ID])

    cg.add(var.set_power_sensor(power_sensor))
    cg.add(var.set_buffer(config[CONF_BUFFER]))
    cg.add(var.set_min_power_demand(config[CONF_MIN_POWER_DEMAND]))
    cg.add(var.set_max_power_demand(config[CONF_MAX_POWER_DEMAND]))
    cg.add(
        var.set_zero_output_on_min_power_demand(
            config[CONF_ZERO_OUTPUT_ON_MIN_POWER_DEMAND]
        )
    )
    cg.add(var.set_power_demand_divider(config[CONF_POWER_DEMAND_DIVIDER]))
    cg.add(
        var.set_power_sensor_inactivity_timeout(
            config[CONF_POWER_SENSOR_INACTIVITY_TIMEOUT]
        )
    )
    cg.add(var.set_power_demand_calculation(config[CONF_POWER_DEMAND_CALCULATION]))
    cg.add(
        var.set_power_demand_compensation_timeout_ms(
            config[CONF_MAX_POWER_SENSOR_LATENCY_MS]
        )
    )

    if CONF_OPERATION_STATUS_ID in config:
        operation_status_sensor = await cg.get_variable(
            config[CONF_OPERATION_STATUS_ID]
        )
        cg.add(var.set_operation_status_sensor(operation_status_sensor))
