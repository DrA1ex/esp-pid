#pragma once

#include <GyverDS18Single.h>
#include "lib/misc/timer.h"

#include "./base.h"
#include "constants.h"

struct __attribute ((packed)) DSx18SensorConfig {
    uint8_t pin = DSX18_PIN;
    uint8_t resolution = 10;
    bool parasite = false;
};

class DSx18Sensor : public SensorBase {
    Timer &_timer;

    DSx18SensorConfig _config;
    GyverDS18Single _sensor;

    uint16_t _update_interval = 0;
    uint32_t _last_request = 0;

    mutable bool _has_value = false;
    float _last_value = 0;

public:
    DSx18Sensor(Timer &timer, const uint8_t *data);

    void begin() override;

    [[nodiscard]] bool has_value() const override { return _has_value; }

    [[nodiscard]] float get_value() const override {
        _has_value = false;
        return _last_value;
    }

protected:
    void request_value();
    void update_value();
};
