#pragma once

#include <lib/utils/enum.h>

MAKE_ENUM(ControlType, uint8_t,
    PWM_VALUE, 0,
);

class ControlBase {
public:
    virtual void begin() = 0;

    [[nodiscard]] virtual float get_value() const = 0;
    virtual void set_value(float value) = 0;

    virtual ~ControlBase() = default;
};
