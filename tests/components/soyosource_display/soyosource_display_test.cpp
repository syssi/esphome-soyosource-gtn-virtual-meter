#include <gtest/gtest.h>
#include "common.h"

namespace esphome::soyosource_display::testing {

// ── Soyo (WiFi version) status ────────────────────────────────────────────────

TEST(SoyoStatusWifiVersionTest, OutputPower) {
  TestableSoyosourceDisplay d;
  sensor::Sensor output_power;
  d.set_output_power_sensor(&output_power);

  d.on_soyo_status(SOYO_STATUS_WIFI_VERSION);

  EXPECT_FLOAT_EQ(output_power.state, 900.0f);
}

TEST(SoyoStatusWifiVersionTest, BatteryVoltageAndCurrent) {
  TestableSoyosourceDisplay d;
  sensor::Sensor battery_voltage, battery_current, battery_power;
  d.set_battery_voltage_sensor(&battery_voltage);
  d.set_battery_current_sensor(&battery_current);
  d.set_battery_power_sensor(&battery_power);

  d.on_soyo_status(SOYO_STATUS_WIFI_VERSION);

  EXPECT_NEAR(battery_voltage.state, 46.3f, 0.001f);
  EXPECT_NEAR(battery_current.state, 1.4f, 0.001f);
  EXPECT_NEAR(battery_power.state, 46.3f * 1.4f, 0.01f);
}

TEST(SoyoStatusWifiVersionTest, GridMeasurements) {
  TestableSoyosourceDisplay d;
  sensor::Sensor ac_voltage, ac_frequency;
  d.set_ac_voltage_sensor(&ac_voltage);
  d.set_ac_frequency_sensor(&ac_frequency);

  d.on_soyo_status(SOYO_STATUS_WIFI_VERSION);

  EXPECT_FLOAT_EQ(ac_voltage.state, 247.0f);
  EXPECT_FLOAT_EQ(ac_frequency.state, 50.0f);
}

TEST(SoyoStatusWifiVersionTest, Temperature) {
  TestableSoyosourceDisplay d;
  sensor::Sensor temperature;
  binary_sensor::BinarySensor fan_running;
  d.set_temperature_sensor(&temperature);
  d.set_fan_running_binary_sensor(&fan_running);

  d.on_soyo_status(SOYO_STATUS_WIFI_VERSION);

  EXPECT_NEAR(temperature.state, 22.1f, 0.001f);
  EXPECT_FALSE(fan_running.state);
}

TEST(SoyoStatusWifiVersionTest, OperationModeAndStatus) {
  TestableSoyosourceDisplay d;
  sensor::Sensor op_mode_id, op_status_id;
  text_sensor::TextSensor op_mode_text, op_status_text;
  d.set_operation_mode_id_sensor(&op_mode_id);
  d.set_operation_mode_text_sensor(&op_mode_text);
  d.set_operation_status_id_sensor(&op_status_id);
  d.set_operation_status_text_sensor(&op_status_text);

  d.on_soyo_status(SOYO_STATUS_WIFI_VERSION);

  EXPECT_FLOAT_EQ(op_mode_id.state, 9.0f);
  EXPECT_EQ(op_mode_text.state, "Battery Limit");
  EXPECT_FLOAT_EQ(op_status_id.state, 0.0f);  // Normal
  EXPECT_EQ(op_status_text.state, "Normal");
}

TEST(SoyoStatusWifiVersionTest, LimiterConnectedAndErrors) {
  TestableSoyosourceDisplay d;
  binary_sensor::BinarySensor limiter_connected;
  sensor::Sensor error_bitmask;
  text_sensor::TextSensor errors_text;
  d.set_limiter_connected_binary_sensor(&limiter_connected);
  d.set_error_bitmask_sensor(&error_bitmask);
  d.set_errors_text_sensor(&errors_text);

  d.on_soyo_status(SOYO_STATUS_WIFI_VERSION);

  EXPECT_TRUE(limiter_connected.state);
  EXPECT_FLOAT_EQ(error_bitmask.state, 0.0f);
  EXPECT_EQ(errors_text.state, "");
}

TEST(SoyoStatusWifiVersionTest, NullSensorsDoNotCrash) {
  TestableSoyosourceDisplay d;
  d.on_soyo_status(SOYO_STATUS_WIFI_VERSION);
}

// ── Soyo (Display version) status ────────────────────────────────────────────

TEST(SoyoStatusDisplayVersionTest, OutputPowerUsesEfficiency) {
  TestableSoyosourceDisplay d;
  d.set_protocol(SOYOSOURCE_DISPLAY_VERSION);
  sensor::Sensor output_power, battery_power;
  d.set_output_power_sensor(&output_power);
  d.set_battery_power_sensor(&battery_power);

  d.on_soyo_status(SOYO_STATUS_DISPLAY_VERSION);

  // battery_power = 24.8 × 27.9 = 691.92 W; output = × 0.86956
  EXPECT_NEAR(battery_power.state, 691.92f, 0.1f);
  EXPECT_NEAR(output_power.state, 691.92f * 0.86956f, 1.0f);
}

TEST(SoyoStatusDisplayVersionTest, BatteryMeasurements) {
  TestableSoyosourceDisplay d;
  d.set_protocol(SOYOSOURCE_DISPLAY_VERSION);
  sensor::Sensor battery_voltage, battery_current;
  d.set_battery_voltage_sensor(&battery_voltage);
  d.set_battery_current_sensor(&battery_current);

  d.on_soyo_status(SOYO_STATUS_DISPLAY_VERSION);

  EXPECT_NEAR(battery_voltage.state, 24.8f, 0.001f);
  EXPECT_NEAR(battery_current.state, 27.9f, 0.001f);
}

TEST(SoyoStatusDisplayVersionTest, GridMeasurements) {
  TestableSoyosourceDisplay d;
  d.set_protocol(SOYOSOURCE_DISPLAY_VERSION);
  sensor::Sensor ac_voltage, ac_frequency;
  d.set_ac_voltage_sensor(&ac_voltage);
  d.set_ac_frequency_sensor(&ac_frequency);

  d.on_soyo_status(SOYO_STATUS_DISPLAY_VERSION);

  EXPECT_FLOAT_EQ(ac_voltage.state, 228.0f);
  EXPECT_FLOAT_EQ(ac_frequency.state, 50.0f);
}

TEST(SoyoStatusDisplayVersionTest, Temperature) {
  TestableSoyosourceDisplay d;
  d.set_protocol(SOYOSOURCE_DISPLAY_VERSION);
  sensor::Sensor temperature;
  binary_sensor::BinarySensor fan_running;
  d.set_temperature_sensor(&temperature);
  d.set_fan_running_binary_sensor(&fan_running);

  d.on_soyo_status(SOYO_STATUS_DISPLAY_VERSION);

  EXPECT_NEAR(temperature.state, 28.5f, 0.001f);
  EXPECT_FALSE(fan_running.state);
}

TEST(SoyoStatusDisplayVersionTest, OperationModeUsesDisplayVersion) {
  TestableSoyosourceDisplay d;
  d.set_protocol(SOYOSOURCE_DISPLAY_VERSION);
  text_sensor::TextSensor op_mode_text;
  d.set_operation_mode_text_sensor(&op_mode_text);

  d.on_soyo_status(SOYO_STATUS_DISPLAY_VERSION);

  EXPECT_EQ(op_mode_text.state, "Battery Limit");
}

TEST(SoyoStatusDisplayVersionTest, LimiterConnected) {
  TestableSoyosourceDisplay d;
  d.set_protocol(SOYOSOURCE_DISPLAY_VERSION);
  binary_sensor::BinarySensor limiter_connected;
  d.set_limiter_connected_binary_sensor(&limiter_connected);

  d.on_soyo_status(SOYO_STATUS_DISPLAY_VERSION);

  EXPECT_TRUE(limiter_connected.state);
}

// ── Soyo settings (WiFi version) ─────────────────────────────────────────────

TEST(SoyoSettingsWifiVersionTest, Voltages) {
  TestableSoyosourceDisplay d;
  TestableNumber start_voltage, shutdown_voltage;
  d.set_start_voltage_number(&start_voltage);
  d.set_shutdown_voltage_number(&shutdown_voltage);

  d.on_soyo_settings(SOYO_SETTINGS_WIFI_VERSION);

  EXPECT_FLOAT_EQ(start_voltage.state, 48.0f);
  EXPECT_FLOAT_EQ(shutdown_voltage.state, 45.0f);
}

TEST(SoyoSettingsWifiVersionTest, PowerAndDelay) {
  TestableSoyosourceDisplay d;
  TestableNumber output_power_limit, start_delay;
  d.set_output_power_limit_number(&output_power_limit);
  d.set_start_delay_number(&start_delay);

  d.on_soyo_settings(SOYO_SETTINGS_WIFI_VERSION);

  EXPECT_FLOAT_EQ(output_power_limit.state, 900.0f);
  EXPECT_FLOAT_EQ(start_delay.state, 6.0f);
}

TEST(SoyoSettingsWifiVersionTest, OperationModeStoredInCurrentSettings) {
  TestableSoyosourceDisplay d;

  d.on_soyo_settings(SOYO_SETTINGS_WIFI_VERSION);

  EXPECT_EQ(d.get_current_settings().OperationMode, 0x01);  // PV
  EXPECT_EQ(d.get_current_settings().StartVoltage, 48);
  EXPECT_EQ(d.get_current_settings().ShutdownVoltage, 45);
  EXPECT_EQ(d.get_current_settings().OutputPowerLimit, 90);  // stored × 10 = 900 W
  EXPECT_EQ(d.get_current_settings().StartDelay, 6);
}

// ── Soyo settings (Display version) ──────────────────────────────────────────

TEST(SoyoSettingsDisplayVersionTest, Voltages) {
  TestableSoyosourceDisplay d;
  d.set_protocol(SOYOSOURCE_DISPLAY_VERSION);
  TestableNumber start_voltage, shutdown_voltage;
  d.set_start_voltage_number(&start_voltage);
  d.set_shutdown_voltage_number(&shutdown_voltage);

  d.on_soyo_settings(SOYO_SETTINGS_DISPLAY_VERSION);

  EXPECT_FLOAT_EQ(start_voltage.state, 30.0f);
  EXPECT_FLOAT_EQ(shutdown_voltage.state, 28.0f);
}

TEST(SoyoSettingsDisplayVersionTest, PowerAndDelay) {
  TestableSoyosourceDisplay d;
  d.set_protocol(SOYOSOURCE_DISPLAY_VERSION);
  TestableNumber output_power_limit, start_delay;
  d.set_output_power_limit_number(&output_power_limit);
  d.set_start_delay_number(&start_delay);

  d.on_soyo_settings(SOYO_SETTINGS_DISPLAY_VERSION);

  EXPECT_FLOAT_EQ(output_power_limit.state, 700.0f);
  EXPECT_FLOAT_EQ(start_delay.state, 6.0f);
}

TEST(SoyoSettingsDisplayVersionTest, OperationModeStoredInCurrentSettings) {
  TestableSoyosourceDisplay d;
  d.set_protocol(SOYOSOURCE_DISPLAY_VERSION);

  d.on_soyo_settings(SOYO_SETTINGS_DISPLAY_VERSION);

  EXPECT_EQ(d.get_current_settings().OperationMode, 0x02);  // BatCP
}

// ── MS51 status ───────────────────────────────────────────────────────────────

TEST(Ms51StatusTest, StandbyWithNoLoad) {
  TestableSoyosourceDisplay d;
  sensor::Sensor battery_voltage, battery_current, output_power;
  d.set_battery_voltage_sensor(&battery_voltage);
  d.set_battery_current_sensor(&battery_current);
  d.set_output_power_sensor(&output_power);

  d.on_ms51_status(MS51_STATUS);

  EXPECT_FLOAT_EQ(battery_voltage.state, 0.0f);
  EXPECT_FLOAT_EQ(battery_current.state, 0.0f);
  EXPECT_FLOAT_EQ(output_power.state, 0.0f);
}

TEST(Ms51StatusTest, GridMeasurements) {
  TestableSoyosourceDisplay d;
  sensor::Sensor ac_voltage, ac_frequency;
  d.set_ac_voltage_sensor(&ac_voltage);
  d.set_ac_frequency_sensor(&ac_frequency);

  d.on_ms51_status(MS51_STATUS);

  EXPECT_FLOAT_EQ(ac_voltage.state, 231.0f);
  EXPECT_FLOAT_EQ(ac_frequency.state, 50.0f);
}

TEST(Ms51StatusTest, Temperature) {
  TestableSoyosourceDisplay d;
  sensor::Sensor temperature;
  binary_sensor::BinarySensor fan_running;
  d.set_temperature_sensor(&temperature);
  d.set_fan_running_binary_sensor(&fan_running);

  d.on_ms51_status(MS51_STATUS);

  EXPECT_FLOAT_EQ(temperature.state, 25.0f);
  EXPECT_FALSE(fan_running.state);
}

TEST(Ms51StatusTest, OperationModeAndStatus) {
  TestableSoyosourceDisplay d;
  sensor::Sensor op_mode_id, op_status_id;
  text_sensor::TextSensor op_mode_text, op_status_text;
  d.set_operation_mode_id_sensor(&op_mode_id);
  d.set_operation_mode_text_sensor(&op_mode_text);
  d.set_operation_status_id_sensor(&op_status_id);
  d.set_operation_status_text_sensor(&op_status_text);

  d.on_ms51_status(MS51_STATUS);

  EXPECT_FLOAT_EQ(op_mode_id.state, 13.0f);  // 0x0D
  EXPECT_EQ(op_mode_text.state, "Battery Limit");
  EXPECT_FLOAT_EQ(op_status_id.state, 2.0f);  // Standby
  EXPECT_EQ(op_status_text.state, "Standby");
}

TEST(Ms51StatusTest, ErrorBitmaskAndText) {
  TestableSoyosourceDisplay d;
  sensor::Sensor error_bitmask;
  text_sensor::TextSensor errors_text;
  binary_sensor::BinarySensor limiter_connected;
  d.set_error_bitmask_sensor(&error_bitmask);
  d.set_errors_text_sensor(&errors_text);
  d.set_limiter_connected_binary_sensor(&limiter_connected);

  d.on_ms51_status(MS51_STATUS);

  EXPECT_FLOAT_EQ(error_bitmask.state, 2.0f);  // bit 1: DC voltage too low
  EXPECT_EQ(errors_text.state, "DC voltage too low");
  EXPECT_FALSE(limiter_connected.state);
}

TEST(Ms51StatusTest, TotalEnergy) {
  TestableSoyosourceDisplay d;
  sensor::Sensor total_energy;
  d.set_total_energy_sensor(&total_energy);

  d.on_ms51_status(MS51_STATUS);

  EXPECT_FLOAT_EQ(total_energy.state, 0.0f);
}

TEST(Ms51StatusTest, NullSensorsDoNotCrash) {
  TestableSoyosourceDisplay d;
  d.on_ms51_status(MS51_STATUS);
}

// ── MS51 settings ─────────────────────────────────────────────────────────────

TEST(Ms51SettingsTest, Voltages) {
  TestableSoyosourceDisplay d;
  TestableNumber start_voltage, shutdown_voltage;
  d.set_start_voltage_number(&start_voltage);
  d.set_shutdown_voltage_number(&shutdown_voltage);

  d.on_ms51_settings(MS51_SETTINGS);

  EXPECT_FLOAT_EQ(start_voltage.state, 49.0f);
  EXPECT_FLOAT_EQ(shutdown_voltage.state, 47.0f);
}

TEST(Ms51SettingsTest, PowerAndDelay) {
  TestableSoyosourceDisplay d;
  TestableNumber output_power_limit, start_delay;
  d.set_output_power_limit_number(&output_power_limit);
  d.set_start_delay_number(&start_delay);

  d.on_ms51_settings(MS51_SETTINGS);

  EXPECT_FLOAT_EQ(output_power_limit.state, 900.0f);
  EXPECT_FLOAT_EQ(start_delay.state, 6.0f);
}

TEST(Ms51SettingsTest, OperationModeStoredInCurrentSettings) {
  TestableSoyosourceDisplay d;

  d.on_ms51_settings(MS51_SETTINGS);

  // op_mode_raw = 0xD3 >> 4 = 0x0D → Battery Limit (WiFi version → 0x12)
  EXPECT_EQ(d.get_current_settings().OperationMode, 0x12);
  EXPECT_EQ(d.get_current_settings().StartVoltage, 49);
  EXPECT_EQ(d.get_current_settings().ShutdownVoltage, 47);
  EXPECT_EQ(d.get_current_settings().OutputPowerLimit, 90);
  EXPECT_EQ(d.get_current_settings().StartDelay, 6);
}

// ── MS51 V2 settings ──────────────────────────────────────────────────────────

TEST(Ms51V2SettingsTest, Voltages) {
  TestableSoyosourceDisplay d;
  TestableNumber start_voltage, shutdown_voltage;
  d.set_start_voltage_number(&start_voltage);
  d.set_shutdown_voltage_number(&shutdown_voltage);

  d.on_ms51_v2_settings(MS51_V2_SETTINGS);

  EXPECT_FLOAT_EQ(start_voltage.state, 27.0f);
  EXPECT_FLOAT_EQ(shutdown_voltage.state, 25.0f);
}

TEST(Ms51V2SettingsTest, PowerAndDelay) {
  TestableSoyosourceDisplay d;
  TestableNumber output_power_limit, start_delay;
  d.set_output_power_limit_number(&output_power_limit);
  d.set_start_delay_number(&start_delay);

  d.on_ms51_v2_settings(MS51_V2_SETTINGS);

  EXPECT_FLOAT_EQ(output_power_limit.state, 50.0f);
  EXPECT_FLOAT_EQ(start_delay.state, 10.0f);
}

TEST(Ms51V2SettingsTest, OperationModeStoredInCurrentSettings) {
  TestableSoyosourceDisplay d;

  d.on_ms51_v2_settings(MS51_V2_SETTINGS);

  // op_mode_raw = 0xD3 >> 4 = 0x0D → Battery Limit (WiFi version → 0x12)
  EXPECT_EQ(d.get_current_settings().OperationMode, 0x12);
  EXPECT_EQ(d.get_current_settings().StartVoltage, 27);
  EXPECT_EQ(d.get_current_settings().ShutdownVoltage, 25);
  EXPECT_EQ(d.get_current_settings().OutputPowerLimit, 5);  // stored ×10 = 50 W
  EXPECT_EQ(d.get_current_settings().StartDelay, 10);
}

TEST(Ms51V2SettingsTest, NullSensorsDoNotCrash) {
  TestableSoyosourceDisplay d;
  d.on_ms51_v2_settings(MS51_V2_SETTINGS);
}

}  // namespace esphome::soyosource_display::testing
