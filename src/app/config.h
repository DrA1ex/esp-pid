#pragma once

#include <memory>

#include <lib/network/wifi.h>

#include "credentials.h"
#include "constants.h"
#include "enum.h"
#include "controls/base.h"
#include "controls/pwm_control.h"
#include "sensors/base.h"
#include "sensors/dsx18_sensor.h"


typedef char ConfigString[CONFIG_STRING_SIZE];

struct __attribute ((packed)) SysConfig {
    ConfigString mdns_name{MDNS_NAME};

    WifiMode wifi_mode = WIFI_MODE;
    ConfigString wifi_ssid{WIFI_SSID};
    ConfigString wifi_password{WIFI_PASSWORD};

    uint32_t wifi_connection_check_interval = WIFI_CONNECTION_CHECK_INTERVAL;
    uint32_t wifi_max_connection_attempt_interval = WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL;

    float time_zone = TIME_ZONE;

    bool mqtt = MQTT;
    ConfigString mqtt_host = MQTT_HOST;
    uint16_t mqtt_port = MQTT_PORT;
    ConfigString mqtt_user = MQTT_USER;
    ConfigString mqtt_password = MQTT_PASSWORD;
};

struct __attribute ((packed)) NightModeConfig {
    bool enabled = false;

    uint32_t start_time = 0;
    uint32_t end_time = (uint32_t) 10 * 60 * 60;
};

struct __attribute ((packed)) SensorConfig {
    SensorType type;
    uint8_t data[1024];

    SensorConfig() : type(SensorType::DSX18X), data{} {
        DSx18SensorConfig ds_config;
        memcpy(data, &ds_config, sizeof(DSx18SensorConfig));
    }
};

struct __attribute ((packed)) ControlConfig {
    ControlType type;
    uint8_t data[1024];

    ControlConfig() : type(ControlType::PWM_VALUE), data{} {
        PwmControlConfig pwn_config;
        memcpy(data, &pwn_config, sizeof(PwmControlConfig));
    }
};

struct __attribute__((packed)) PidConfig {
    float target = 30;        // Setpoint
    uint16_t interval = 1000; // Sampling period in ms (dt)

    float p = 1;    // Proportional coefficient (Kp)
    float i = 0.05; // Integral coefficient (Ki)
    float d = 0;    // Differential coefficient (Kd)

    float kbc = 0;      // Back calculation coefficient (Kbc)
    float out_max = 255; // Maximum output
    float out_min = 0;   // Minimum output

    ProportionalMode p_mode = ProportionalMode::P_ERROR;   // Proportional mode
    IntegralMode i_mode = IntegralMode::I_KI_OUTSIDE;      // Integral mode
    IntegralLimitMode i_limit = IntegralLimitMode::I_NONE; // Integral limit mode
    DifferentialMode d_mode = DifferentialMode::D_ERROR;   // Differential mode
    DirectionMode direction = DirectionMode::PID_FORWARD; // Direction mode
};

struct __attribute ((packed)) RegulatorConfig {
    SensorConfig sensor{};
    ControlConfig control{};
    PidConfig pid{};
};

struct __attribute ((packed)) Config {
    bool power = true;

    RegulatorConfig regulator{};
    NightModeConfig night_mode{};

    SysConfig sys_config{};
};

struct __attribute ((packed)) HistoryEntry {
    float sensor = NAN;
    float control = NAN;
};

struct __attribute ((packed)) DataHistory {
    const uint16_t count = HISTORY_COUNT;

    uint16_t index = 0;
    HistoryEntry entries[HISTORY_COUNT]{};
};

struct __attribute ((packed)) RuntimeInfo {
    float sensor_value;
    float control_value;

    DataHistory history;
};
