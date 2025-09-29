#include "pwm_control.h"

#include <Arduino.h>

#include "sys_constants.h"
#include "lib/debug.h"

PwmControl::PwmControl(Timer &timer, const uint8_t *data): _timer(timer) {
    memcpy(&_config, data, sizeof(_config));
}

void PwmControl::begin() {
    pinMode(_config.pin, OUTPUT);

    update_value(_value);
    set_state(_state);

    _timer.add_interval([this](auto) {
        this->update();
    }, CONTROL_LOOP_INTERVAL);
}

void PwmControl::update() {
    if (_state && millis() - _state_change_time >= _on_time) {
        set_state(false);
        if (_off_time > 0) write_pin();
    } else if (!_state && millis() - _state_change_time >= _off_time) {
        set_state(true);
        if (_on_time > 0) write_pin();
    }
}

void PwmControl::update_value(float value) {
    _current_value = value;
    _on_time = (uint16_t) (_value * _config.period);
    _off_time = _config.period - _on_time;

    D_PRINTF("Setup PWM (%u): %f; ON_TIME: %u, OFF_TIME: %u\r\n", _config.pin, value, _on_time, _off_time);
}

void PwmControl::set_state(bool state) {
    _state = state;
    _state_change_time = millis();

    if (_current_value != _value) {
        update_value(_value);
    }
}

void PwmControl::write_pin() { // NOLINT(*-make-member-function-const)
    digitalWrite(_config.pin, _state ? HIGH : LOW);
}
