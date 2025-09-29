#pragma once

#include <Arduino.h>
#include "lib/misc/timer.h"

#include "./base.h"
#include "constants.h"

struct __attribute ((packed)) AnalogSensorConfig {
    uint8_t pin = ANALOG_PIN;
    uint8_t resolution = 8;
};

class AnalogSensor : SensorBase {
    AnalogSensorConfig _config;

    uint16_t _max_value = 0;

public:
    AnalogSensor(Timer &timer, const uint8_t *data) {
        memcpy(&_config, data, sizeof(_config));
    }

    void begin() override {
        // TODO: use different resolution for each sensor
        analogReadResolution(_config.resolution);
        _max_value = 2 << (_config.resolution - 1);
    }

    [[nodiscard]] bool has_value() const override { return true; }

    [[nodiscard]] float get_value() const override {
        return (float) analogRead(_config.pin) / _max_value;
    }
};
