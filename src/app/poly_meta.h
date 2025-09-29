#pragma once

#include "app/metadata.h"

#include "controls/pwm_control.h"
#include "sensors/dsx18_sensor.h"

DECLARE_META(PwmControlConfigMeta, AppMetaProperty,
    MEMBER(Parameter<uint8_t>, pin),
    MEMBER(Parameter<uint16_t>, period)
)

DECLARE_META(AnalogSensorConfigMeta, AppMetaProperty,
    MEMBER(Parameter<uint8_t>, pin),
    MEMBER(Parameter<uint8_t>, resolution),
)

DECLARE_META(DSx18SensorConfigMeta, AppMetaProperty,
    MEMBER(Parameter<uint8_t>, pin),
    MEMBER(Parameter<uint8_t>, resolution),
    MEMBER(Parameter<bool>, parasite)
)

inline PwmControlConfigMeta build_pwm_control_metadata(PwmControlConfig &config) {
    return {
        .pin = {
            PacketType::PWM_CONTROL_PIN,
            &config.pin
        },
        .period = {
            PacketType::PWM_CONTROL_PERIOD,
            &config.period
        }
    };
}

inline AnalogSensorConfigMeta build_analog_sensor_metadata(PwmControlConfig &config) {
    return {
        .pin = {
            PacketType::ANALOG_SENSOR_PIN,
            &config.pin
        },
        .resolution = {
            PacketType::ANALOG_SENSOR_RESOLUTION,
            &config.period
        }
    };
}

inline DSx18SensorConfigMeta build_dsx18_sensor_metadata(DSx18SensorConfig &config) {
    return {
        .pin = {
            PacketType::DSX18_SENSOR_PIN,
            &config.pin
        },
        .resolution = {
            PacketType::DSX18_SENSOR_RESOLUTION,
            &config.resolution
        },
        .parasite = {
            PacketType::DSX18_SENSOR_PARASITE,
            &config.parasite
        }
    };
}
