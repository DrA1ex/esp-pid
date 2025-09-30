#pragma once

#include "app/metadata.h"

#include "controls/pwm_control.h"
#include "sensors/analog_sensor.h"
#include "sensors/dsx18_sensor.h"

class AbstractMetaHolder {
public:
    virtual ~AbstractMetaHolder() = default;
    virtual void visit(const MetaVisitFn &fn) = 0;
};

template<typename T>
class MetaHolder : public AbstractMetaHolder {
    T _t;

public:
    MetaHolder(T &&t): _t(t) {}

    void visit(const MetaVisitFn &fn) override { _t.visit(fn); }
};


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

inline MetaHolder<PwmControlConfigMeta> build_pwm_control_metadata(PwmControlConfig &config) {
    return MetaHolder(PwmControlConfigMeta{
        .pin = {
            PacketType::PWM_CONTROL_PIN,
            &config.pin
        },
        .period = {
            PacketType::PWM_CONTROL_PERIOD,
            &config.period
        }
    });
}

inline MetaHolder<AnalogSensorConfigMeta> build_analog_sensor_metadata(AnalogSensorConfig &config) {
    return MetaHolder(AnalogSensorConfigMeta{
        .pin = {
            PacketType::ANALOG_SENSOR_PIN,
            &config.pin
        },
        .resolution = {
            PacketType::ANALOG_SENSOR_RESOLUTION,
            &config.resolution
        }
    });
}

inline MetaHolder<DSx18SensorConfigMeta> build_dsx18_sensor_metadata(DSx18SensorConfig &config) {
    return MetaHolder(DSx18SensorConfigMeta{
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
    });
}
