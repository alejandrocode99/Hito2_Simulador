#ifndef GATEWAY_ECU_HPP
#define GATEWAY_ECU_HPP

#include "Types.hpp"

class GatewayECU {
public:
    GatewayECU() = default;

    // Validate individual ranges and check for physical signal plausibility
    static SignalValidation validateSignals(const SignalData& signals);

    // Generate random signals within safe ranges
    static SignalData generateRandomSignals();

    // Range validation helper functions
    static bool validateSpeed(double speed);
    static bool validateRPM(int rpm);
    static bool validateTemperature(double temp);
    static bool validateThrottle(double throttle);
    static bool validateBattery(double voltage);
};

#endif // GATEWAY_ECU_HPP
