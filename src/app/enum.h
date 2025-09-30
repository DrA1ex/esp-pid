#pragma once

#include <lib/utils/enum.h>

MAKE_ENUM_AUTO(AppState, uint8_t,
    UNINITIALIZED,
    INITIALIZATION,
    ACTIVE,
    INACTIVE,
);

MAKE_ENUM_AUTO(ProportionalMode, uint8_t,
    P_ERROR, // Proportional to error (default)
    P_INPUT  // Proportional to input
);

// Integral mode
MAKE_ENUM_AUTO(IntegralMode, uint8_t,
    I_KI_OUTSIDE, // Coefficient outside integral (default)
    I_KI_INSIDE   // Coefficient inside integral
);

// Integral limit mode (can be combined)
MAKE_ENUM(IntegralLimitMode, uint8_t,
    I_NONE, 0,      // No integral limiting
    I_SATURATE, 1,  // Conditional Integration
    I_BACK_CALC, 2, // Back Calculation
    I_RESET, 4      // Reset integral on setpoint reached
);

// Differential mode
MAKE_ENUM_AUTO(DifferentialMode, uint8_t,
    D_ERROR, // Differentiate error (default)
    D_INPUT  // Differentiate input
);

// Direction mode
MAKE_ENUM_AUTO(DirectionMode, uint8_t,
    PID_FORWARD, // Direct control (default)
    PID_REVERSE  // Reverse control
);
