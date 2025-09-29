#pragma once

#include <lib/network/wifi.h>
#include <lib/utils/enum.h>

#include "credentials.h"
#include "constants.h"
#include "controls/base.h"
#include "controls/pwm_control.h"
#include "sensors/base.h"
#include "sensors/dsx18_sensor.h"

MAKE_ENUM_AUTO(AppState, uint8_t,
    UNINITIALIZED,
    INITIALIZATION,
    ACTIVE,
    INACTIVE,
);

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
        std::memcpy(data, &ds_config, sizeof(DSx18SensorConfig));
    }
};

struct __attribute ((packed)) ControlConfig {
    ControlType type;
    uint8_t data[1024];

    ControlConfig() : type(ControlType::PWM_VALUE), data{} {
        PwmControlConfig pwn_config;
        std::memcpy(data, &pwn_config, sizeof(PwmControlConfig));
    }
};

struct __attribute ((packed)) PidConfig {
    float target = 30;

    float p = 1;
    float i = 0.05;
    float d = 0;

    uint16_t interval = 1000;
    bool reverse = false;
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

struct __attribute ((packed)) RuntimeInfo {
    float sensor_value;
    float control_value;
};
