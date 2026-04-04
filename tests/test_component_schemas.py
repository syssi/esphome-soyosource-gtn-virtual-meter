"""Schema structure tests for soyosource ESPHome component modules."""

import os
import sys

sys.path.insert(0, os.path.join(os.path.dirname(__file__), ".."))

import components.soyosource_display as hub_display  # noqa: E402
from components.soyosource_display import (  # noqa: E402
    binary_sensor as display_binary_sensor,
    button as display_button,  # noqa: E402
    number as display_number,  # noqa: E402
    sensor as display_sensor,
    text_sensor as display_text_sensor,
)
import components.soyosource_inverter as hub_inverter  # noqa: E402
from components.soyosource_inverter import (  # noqa: E402
    binary_sensor as inverter_binary_sensor,
    sensor as inverter_sensor,
    text_sensor as inverter_text_sensor,
)
import components.soyosource_virtual_meter as hub_vm  # noqa: E402
from components.soyosource_virtual_meter import (  # noqa: E402
    number as vm_number,  # noqa: E402
    switch as vm_switch,  # noqa: E402
    text_sensor as vm_text_sensor,
)


class TestHubConstants:
    def test_conf_ids_defined(self):
        assert hub_display.CONF_SOYOSOURCE_DISPLAY_ID == "soyosource_display_id"
        assert hub_inverter.CONF_SOYOSOURCE_INVERTER_ID == "soyosource_inverter_id"
        assert hub_vm.CONF_SOYOSOURCE_VIRTUAL_METER_ID == "soyosource_virtual_meter_id"


class TestDisplaySensorLists:
    def test_sensor_defs_completeness(self):
        assert display_sensor.CONF_BATTERY_VOLTAGE in display_sensor.SENSOR_DEFS
        assert display_sensor.CONF_OUTPUT_POWER in display_sensor.SENSOR_DEFS
        assert len(display_sensor.SENSOR_DEFS) == 11

    def test_binary_sensor_defs_dict(self):
        assert (
            display_binary_sensor.CONF_FAN_RUNNING
            in display_binary_sensor.BINARY_SENSOR_DEFS
        )
        assert (
            display_binary_sensor.CONF_LIMITER_CONNECTED
            in display_binary_sensor.BINARY_SENSOR_DEFS
        )
        assert len(display_binary_sensor.BINARY_SENSOR_DEFS) == 2

    def test_text_sensors_list(self):
        assert display_text_sensor.CONF_ERRORS in display_text_sensor.TEXT_SENSORS
        assert (
            display_text_sensor.CONF_OPERATION_MODE in display_text_sensor.TEXT_SENSORS
        )
        assert (
            display_text_sensor.CONF_OPERATION_STATUS
            in display_text_sensor.TEXT_SENSORS
        )
        assert len(display_text_sensor.TEXT_SENSORS) == 3


class TestDisplayButtonConstants:
    def test_buttons_dict(self):
        from esphome.const import CONF_RESTART

        assert CONF_RESTART in display_button.BUTTONS
        assert len(display_button.BUTTONS) == 1


class TestDisplayNumberConstants:
    def test_numbers_dict(self):
        assert display_number.CONF_START_VOLTAGE in display_number.NUMBERS
        assert display_number.CONF_SHUTDOWN_VOLTAGE in display_number.NUMBERS
        assert display_number.CONF_OUTPUT_POWER_LIMIT in display_number.NUMBERS
        assert len(display_number.NUMBERS) == 4

    def test_number_addresses_are_unique(self):
        addresses = list(display_number.NUMBERS.values())
        assert len(addresses) == len(set(addresses))


class TestInverterSensorLists:
    def test_sensor_defs_completeness(self):
        assert inverter_sensor.CONF_BATTERY_VOLTAGE in inverter_sensor.SENSOR_DEFS
        assert len(inverter_sensor.SENSOR_DEFS) == 7

    def test_binary_sensor_defs_dict(self):
        assert (
            inverter_binary_sensor.CONF_FAN_RUNNING
            in inverter_binary_sensor.BINARY_SENSOR_DEFS
        )
        assert len(inverter_binary_sensor.BINARY_SENSOR_DEFS) == 1

    def test_text_sensors_list(self):
        assert (
            inverter_text_sensor.CONF_OPERATION_STATUS
            in inverter_text_sensor.TEXT_SENSORS
        )
        assert len(inverter_text_sensor.TEXT_SENSORS) == 1


class TestVirtualMeterConstants:
    def test_text_sensors_list(self):
        assert vm_text_sensor.CONF_OPERATION_MODE in vm_text_sensor.TEXT_SENSORS
        assert len(vm_text_sensor.TEXT_SENSORS) == 1

    def test_switches_list(self):
        assert vm_switch.CONF_MANUAL_MODE in vm_switch.SWITCHES
        assert vm_switch.CONF_EMERGENCY_POWER_OFF in vm_switch.SWITCHES
        assert len(vm_switch.SWITCHES) == 2

    def test_numbers_dict(self):
        assert vm_number.CONF_MANUAL_POWER_DEMAND in vm_number.NUMBERS
        assert vm_number.CONF_MAX_POWER_DEMAND in vm_number.NUMBERS
        assert vm_number.CONF_BUFFER in vm_number.NUMBERS
        assert vm_number.CONF_POWER_DEMAND_DIVIDER in vm_number.NUMBERS
        assert len(vm_number.NUMBERS) == 4

    def test_number_addresses_are_unique(self):
        addresses = list(vm_number.NUMBERS.values())
        assert len(addresses) == len(set(addresses))
