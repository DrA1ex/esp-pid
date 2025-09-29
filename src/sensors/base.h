#pragma once

#include <lib/utils/enum.h>

MAKE_ENUM(SensorType, uint8_t,
    ANALOG_VALUE, 0,
    DSX18X, 1,
);

class SensorBase {
public:
    virtual void begin() = 0;

    [[nodiscard]] virtual bool has_value() const = 0;
    [[nodiscard]] virtual float get_value() const = 0;

    virtual ~SensorBase() = default;
};
