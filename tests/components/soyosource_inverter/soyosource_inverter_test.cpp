#include <gtest/gtest.h>
#include "common.h"

namespace esphome::soyosource_inverter::testing {

// ── Normal operation ──────────────────────────────────────────────────────────

TEST(NormalStatusTest, BatteryMeasurements) {
  TestableSoyosourceInverter inv;
  sensor::Sensor battery_voltage, battery_current, battery_power;
  inv.set_battery_voltage_sensor(&battery_voltage);
  inv.set_battery_current_sensor(&battery_current);
  inv.set_battery_power_sensor(&battery_power);

  inv.on_soyosource_modbus_data(RS485_STATUS_NORMAL);

  EXPECT_NEAR(battery_voltage.state, 47.5f, 0.001f);
  EXPECT_NEAR(battery_current.state, 16.1f, 0.001f);
  EXPECT_NEAR(battery_power.state, 47.5f * 16.1f, 0.01f);
}

TEST(NormalStatusTest, GridMeasurements) {
  TestableSoyosourceInverter inv;
  sensor::Sensor ac_voltage, ac_frequency;
  inv.set_ac_voltage_sensor(&ac_voltage);
  inv.set_ac_frequency_sensor(&ac_frequency);

  inv.on_soyosource_modbus_data(RS485_STATUS_NORMAL);

  EXPECT_FLOAT_EQ(ac_voltage.state, 221.0f);
  EXPECT_FLOAT_EQ(ac_frequency.state, 50.0f);
}

TEST(NormalStatusTest, Temperature) {
  TestableSoyosourceInverter inv;
  sensor::Sensor temperature;
  binary_sensor::BinarySensor fan_running;
  inv.set_temperature_sensor(&temperature);
  inv.set_fan_running_binary_sensor(&fan_running);

  inv.on_soyosource_modbus_data(RS485_STATUS_NORMAL);

  EXPECT_NEAR(temperature.state, 41.4f, 0.001f);
  EXPECT_FALSE(fan_running.state);
}

TEST(NormalStatusTest, OperationStatus) {
  TestableSoyosourceInverter inv;
  sensor::Sensor op_status_id;
  text_sensor::TextSensor op_status_text;
  inv.set_operation_status_id_sensor(&op_status_id);
  inv.set_operation_status_text_sensor(&op_status_text);

  inv.on_soyosource_modbus_data(RS485_STATUS_NORMAL);

  EXPECT_FLOAT_EQ(op_status_id.state, 0.0f);
  EXPECT_EQ(op_status_text.state, "Normal");
}

TEST(NormalStatusTest, NullSensorsDoNotCrash) {
  TestableSoyosourceInverter inv;
  inv.on_soyosource_modbus_data(RS485_STATUS_NORMAL);
}

// ── Error / battery mode ──────────────────────────────────────────────────────

TEST(ErrorStatusTest, BatteryMeasurements) {
  TestableSoyosourceInverter inv;
  sensor::Sensor battery_voltage, battery_current, battery_power;
  inv.set_battery_voltage_sensor(&battery_voltage);
  inv.set_battery_current_sensor(&battery_current);
  inv.set_battery_power_sensor(&battery_power);

  inv.on_soyosource_modbus_data(RS485_STATUS_ERROR);

  EXPECT_NEAR(battery_voltage.state, 70.1f, 0.001f);
  EXPECT_NEAR(battery_current.state, 7.0f, 0.001f);
  EXPECT_NEAR(battery_power.state, 70.1f * 7.0f, 0.01f);
}

TEST(ErrorStatusTest, GridMeasurements) {
  TestableSoyosourceInverter inv;
  sensor::Sensor ac_voltage, ac_frequency;
  inv.set_ac_voltage_sensor(&ac_voltage);
  inv.set_ac_frequency_sensor(&ac_frequency);

  inv.on_soyosource_modbus_data(RS485_STATUS_ERROR);

  EXPECT_FLOAT_EQ(ac_voltage.state, 239.0f);
  EXPECT_FLOAT_EQ(ac_frequency.state, 50.0f);
}

TEST(ErrorStatusTest, Temperature) {
  TestableSoyosourceInverter inv;
  sensor::Sensor temperature;
  binary_sensor::BinarySensor fan_running;
  inv.set_temperature_sensor(&temperature);
  inv.set_fan_running_binary_sensor(&fan_running);

  inv.on_soyosource_modbus_data(RS485_STATUS_ERROR);

  EXPECT_NEAR(temperature.state, 27.9f, 0.001f);
  EXPECT_FALSE(fan_running.state);
}

TEST(ErrorStatusTest, OperationStatus) {
  TestableSoyosourceInverter inv;
  sensor::Sensor op_status_id;
  text_sensor::TextSensor op_status_text;
  inv.set_operation_status_id_sensor(&op_status_id);
  inv.set_operation_status_text_sensor(&op_status_text);

  inv.on_soyosource_modbus_data(RS485_STATUS_ERROR);

  EXPECT_FLOAT_EQ(op_status_id.state, 4.0f);
  EXPECT_EQ(op_status_text.state, "Error or battery mode?");
}

// ── Startup state ─────────────────────────────────────────────────────────────

TEST(StartupStatusTest, OperationStatus) {
  TestableSoyosourceInverter inv;
  sensor::Sensor op_status_id;
  text_sensor::TextSensor op_status_text;
  inv.set_operation_status_id_sensor(&op_status_id);
  inv.set_operation_status_text_sensor(&op_status_text);

  inv.on_soyosource_modbus_data(RS485_STATUS_STARTUP);

  EXPECT_FLOAT_EQ(op_status_id.state, 1.0f);
  EXPECT_EQ(op_status_text.state, "Startup");
}

TEST(StartupStatusTest, BatteryVoltageNoCurrent) {
  TestableSoyosourceInverter inv;
  sensor::Sensor battery_voltage, battery_current, battery_power;
  inv.set_battery_voltage_sensor(&battery_voltage);
  inv.set_battery_current_sensor(&battery_current);
  inv.set_battery_power_sensor(&battery_power);

  inv.on_soyosource_modbus_data(RS485_STATUS_STARTUP);

  EXPECT_NEAR(battery_voltage.state, 54.6f, 0.001f);
  EXPECT_FLOAT_EQ(battery_current.state, 0.0f);
  EXPECT_FLOAT_EQ(battery_power.state, 0.0f);
}

TEST(StartupStatusTest, GridMeasurements) {
  TestableSoyosourceInverter inv;
  sensor::Sensor ac_voltage, ac_frequency;
  inv.set_ac_voltage_sensor(&ac_voltage);
  inv.set_ac_frequency_sensor(&ac_frequency);

  inv.on_soyosource_modbus_data(RS485_STATUS_STARTUP);

  EXPECT_FLOAT_EQ(ac_voltage.state, 236.0f);
  EXPECT_FLOAT_EQ(ac_frequency.state, 50.0f);
}

TEST(StartupStatusTest, Temperature) {
  TestableSoyosourceInverter inv;
  sensor::Sensor temperature;
  inv.set_temperature_sensor(&temperature);

  inv.on_soyosource_modbus_data(RS485_STATUS_STARTUP);

  EXPECT_NEAR(temperature.state, 21.7f, 0.001f);
}


}  // namespace esphome::soyosource_inverter::testing
