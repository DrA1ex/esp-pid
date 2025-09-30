#pragma once

#include "lib/misc/timer.h"

#include "./base.h"
#include "constants.h"

struct __attribute ((packed)) PwmControlConfig {
    uint8_t pin = PWM_PIN;
    uint16_t period = 500;
};

class PwmControl : public ControlBase {
    Timer &_timer;

    PwmControlConfig _config;

    float _current_value = 0.0;
    float _value = 0.0;

    bool _state = false;
    uint32_t _state_change_time = 0;

    uint16_t _on_time = 0;
    uint16_t _off_time = 0;

public:
    explicit PwmControl(Timer &timer, const uint8_t *data);
    void begin() override;

    [[nodiscard]] float get_value() const override { return _value; }
    void set_value(float value) override { _value = std::max(0.0f, std::min(value, 1.0f)); }

protected:
    void update();

    void update_value(float value);
    void set_state(bool state);
    void write_pin();

    void on_switch();
};
