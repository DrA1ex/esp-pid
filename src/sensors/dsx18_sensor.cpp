#include "./dsx18_sensor.h"

#include "sys_constants.h"
#include "lib/debug.h"

DSx18Sensor::DSx18Sensor(Timer &timer, const uint8_t *data): _timer(timer) {
    memcpy(&_config, data, sizeof(_config));

    _sensor.setPin(_config.pin);
    _sensor.setResolution(_config.resolution);
    _sensor.setParasite(_config.parasite);

    _update_interval = _sensor.getConversionTime();
}

void DSx18Sensor::begin() {
    request_value();

    _timer.add_interval([this](auto) {
        this->update_value();
    }, SENSOR_LOOP_INTERVAL);
}

void DSx18Sensor::request_value() {
    _sensor.requestTemp();
    _last_request = millis();
}

void DSx18Sensor::update_value() {
    if (millis() - _last_request <= _update_interval) return;

    _has_value = _sensor.readTemp();
    if (_has_value) {
        _last_value = _sensor.getTemp();
    } else {
        D_PRINT("Unable to read temperature");
    }

    _sensor.requestTemp();
}
