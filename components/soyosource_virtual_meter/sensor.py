import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import sensor
from esphome.const import DEVICE_CLASS_POWER, ICON_EMPTY, UNIT_WATT

from . import CONF_SOYOSOURCE_VIRTUAL_METER_ID, SoyosourceVirtualMeter

DEPENDENCIES = ["soyosource_virtual_meter"]

CONF_POWER_DEMAND = "power_demand"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(CONF_SOYOSOURCE_VIRTUAL_METER_ID): cv.use_id(
            SoyosourceVirtualMeter
        ),
        cv.Optional(CONF_POWER_DEMAND): sensor.sensor_schema(
            UNIT_WATT, ICON_EMPTY, 0, DEVICE_CLASS_POWER
        ),
    }
)


def to_code(config):
    hub = yield cg.get_variable(config[CONF_SOYOSOURCE_VIRTUAL_METER_ID])
    if CONF_POWER_DEMAND in config:
        sens = yield sensor.new_sensor(config[CONF_POWER_DEMAND])
        cg.add(hub.set_power_demand_sensor(sens))
