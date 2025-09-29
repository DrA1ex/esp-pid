#pragma once

#include <lib/base/metadata.h>
#include <lib/utils/metadata.h>

#include "app/config.h"
#include "cmd.h"

DECLARE_META_TYPE(AppMetaProperty, PacketType)

DECLARE_META(SensorConfigMeta, AppMetaProperty,
    MEMBER(Parameter<uint8_t>, type),
)

DECLARE_META(ControlConfigMeta, AppMetaProperty,
    MEMBER(Parameter<uint8_t>, type),
)

DECLARE_META(PidConfigMeta, AppMetaProperty,
    MEMBER(Parameter<float>, target),
    MEMBER(Parameter<float>, p),
    MEMBER(Parameter<float>, i),
    MEMBER(Parameter<float>, d),
    MEMBER(Parameter<uint16_t>, interval),
    MEMBER(Parameter<bool>, reverse)
)

DECLARE_META(RegulatorConfigMeta, AppMetaProperty,
    SUB_TYPE(SensorConfigMeta, sensor),
    SUB_TYPE(ControlConfigMeta, control),
    SUB_TYPE(PidConfigMeta, pid)
)

DECLARE_META(NightModeConfigMeta, AppMetaProperty,
    MEMBER(Parameter<bool>, enabled),
    MEMBER(Parameter<uint32_t>, start_time),
    MEMBER(Parameter<uint32_t>, end_time)
)

DECLARE_META(DataConfigMeta, AppMetaProperty,
    MEMBER(ComplexParameter<Config>, config),
    MEMBER(ComplexParameter<RuntimeInfo>, state),

    MEMBER(Parameter<float>, sensor_value),
    MEMBER(Parameter<float>, control_value),
)

DECLARE_META(SysConfigMeta, AppMetaProperty,
    MEMBER(FixedString, mdns_name),
    MEMBER(Parameter<uint8_t>, wifi_mode),
    MEMBER(FixedString, wifi_ssid),
    MEMBER(FixedString, wifi_password),
    MEMBER(Parameter<uint32_t>, wifi_connection_check_interval),
    MEMBER(Parameter<uint32_t>, wifi_max_connection_attempt_interval),
    MEMBER(Parameter<float>, time_zone),
    MEMBER(Parameter<bool>, mqtt),
    MEMBER(FixedString, mqtt_host),
    MEMBER(Parameter<uint16_t>, mqtt_port),
    MEMBER(FixedString, mqtt_user),
    MEMBER(FixedString, mqtt_password)
)

DECLARE_META(ConfigMetadata, AppMetaProperty,
    MEMBER(Parameter<bool>, power),
    SUB_TYPE(RegulatorConfigMeta, regulator),
    SUB_TYPE(NightModeConfigMeta, night_mode),
    SUB_TYPE(SysConfigMeta, sys_config),
    SUB_TYPE(DataConfigMeta, data)
)

inline ConfigMetadata build_metadata(Config &config, RuntimeInfo &runtime_info) {
    return {
        .power = {
            PacketType::POWER,
            MQTT_TOPIC_POWER, MQTT_OUT_TOPIC_POWER,
            &config.power
        },
        .regulator = {
            .sensor = {
                .type = {
                    PacketType::SENSOR_TYPE,
                    &config.regulator.sensor.type
                },
            },
            .control = {
                .type = {
                    PacketType::CONTROL_TYPE,
                    &config.regulator.control.type
                },
            },
            .pid = {
                .target = {
                    PacketType::PID_TARGET,
                    &config.regulator.pid.target
                },
                .p = {
                    PacketType::PID_P,
                    &config.regulator.pid.p
                },
                .i = {
                    PacketType::PID_I,
                    &config.regulator.pid.i
                },
                .d = {
                    PacketType::PID_D,
                    &config.regulator.pid.d
                },
                .interval = {
                    PacketType::PID_INTERVAL,
                    &config.regulator.pid.interval
                },
                .reverse = {
                    PacketType::PID_REVERSE,
                    &config.regulator.pid.reverse
                }
            }
        },
        .night_mode = {
            .enabled = {
                PacketType::NIGHT_MODE_ENABLED,
                MQTT_TOPIC_NIGHT_MODE, MQTT_OUT_TOPIC_NIGHT_MODE,
                &config.night_mode.enabled
            },
            .start_time = {
                PacketType::NIGHT_MODE_START,
                &config.night_mode.start_time
            },
            .end_time = {
                PacketType::NIGHT_MODE_END,
                &config.night_mode.end_time
            }
        },
        .sys_config = {
            .mdns_name = {
                PacketType::SYS_CONFIG_MDNS_NAME,
                {config.sys_config.mdns_name, CONFIG_STRING_SIZE}
            },
            .wifi_mode = {
                PacketType::SYS_CONFIG_WIFI_MODE,
                (uint8_t *) &config.sys_config.wifi_mode
            },
            .wifi_ssid = {
                PacketType::SYS_CONFIG_WIFI_SSID,
                {config.sys_config.wifi_ssid, CONFIG_STRING_SIZE}
            },
            .wifi_password = {
                PacketType::SYS_CONFIG_WIFI_PASSWORD,
                {config.sys_config.wifi_password, CONFIG_STRING_SIZE}
            },
            .wifi_connection_check_interval = {
                PacketType::SYS_CONFIG_WIFI_CONNECTION_CHECK_INTERVAL,
                &config.sys_config.wifi_connection_check_interval
            },
            .wifi_max_connection_attempt_interval = {
                PacketType::SYS_CONFIG_WIFI_MAX_CONNECTION_ATTEMPT_INTERVAL,
                &config.sys_config.wifi_max_connection_attempt_interval
            },
            .time_zone = {
                PacketType::SYS_CONFIG_TIME_ZONE,
                &config.sys_config.time_zone
            },
            .mqtt = {
                PacketType::SYS_CONFIG_MQTT_ENABLED,
                &config.sys_config.mqtt
            },
            .mqtt_host = {
                PacketType::SYS_CONFIG_MQTT_HOST,
                {config.sys_config.mqtt_host, CONFIG_STRING_SIZE}
            },
            .mqtt_port = {
                PacketType::SYS_CONFIG_MQTT_PORT,
                &config.sys_config.mqtt_port
            },
            .mqtt_user = {
                PacketType::SYS_CONFIG_MQTT_USER,
                {config.sys_config.mqtt_user, CONFIG_STRING_SIZE}
            },
            .mqtt_password = {
                PacketType::SYS_CONFIG_MQTT_PASSWORD,
                {config.sys_config.mqtt_password, CONFIG_STRING_SIZE}
            }
        },
        .data = {
            .config = ComplexParameter(&config),
            .state = ComplexParameter(&runtime_info),

            .sensor_value = Parameter(&runtime_info.sensor_value),
            .control_value = Parameter(&runtime_info.control_value),
        }
    };
}
