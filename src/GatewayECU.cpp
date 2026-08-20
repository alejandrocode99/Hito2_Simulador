#include "GatewayECU.hpp"
#include <sstream>

bool GatewayECU::validateSpeed(double speed) {
    return speed >= 0.0 && speed <= 250.0;
}

bool GatewayECU::validateRPM(int rpm) {
    return rpm >= 0 && rpm <= 8000;
}

bool GatewayECU::validateTemperature(double temp) {
    return temp >= -40.0 && temp <= 150.0;
}

bool GatewayECU::validateThrottle(double throttle) {
    return throttle >= 0.0 && throttle <= 100.0;
}

bool GatewayECU::validateBattery(double voltage) {
    return voltage >= 9.0 && voltage <= 16.0;
}

SignalValidation GatewayECU::validateSignals(const SignalData& signals) const {
    SignalValidation val;

    val.speedValid = validateSpeed(signals.speed);
    if (!val.speedValid) {
        val.validationErrors.push_back("Speed out of valid range [0, 250 km/h]: " + std::to_string(signals.speed));
    }

    val.rpmValid = validateRPM(signals.rpm);
    if (!val.rpmValid) {
        val.validationErrors.push_back("RPM out of valid range [0, 8000 rpm]: " + std::to_string(signals.rpm));
    }

    val.temperatureValid = validateTemperature(signals.temperature);
    if (!val.temperatureValid) {
        val.validationErrors.push_back("Temperature out of valid range [-40, 150 C]: " + std::to_string(signals.temperature));
    }

    val.throttleValid = validateThrottle(signals.throttle);
    if (!val.throttleValid) {
        val.validationErrors.push_back("Throttle out of valid range [0, 100 %]: " + std::to_string(signals.throttle));
    }

    val.batteryValid = validateBattery(signals.battery);
    if (!val.batteryValid) {
        val.validationErrors.push_back("Battery voltage out of valid range [9, 16 V]: " + std::to_string(signals.battery));
    }

    // Physical implausibility checks at Gateway level
    if (val.speedValid && val.rpmValid && val.throttleValid) {
        if (signals.speed >= 240.0 && signals.rpm < 1000 && signals.throttle < 10.0) {
            val.speedValid = false;
            val.validationErrors.push_back("Gateway Physical Impossibility: Max speed with low RPM & low Throttle");
        }
        if (signals.rpm > 6000 && signals.throttle < 5.0) {
            val.rpmValid = false;
            val.validationErrors.push_back("Gateway Invalid Condition: High RPM (>6000) with minimal Throttle (<5%)");
        }
    }

    val.allValid = val.speedValid && val.rpmValid && val.temperatureValid && val.throttleValid && val.batteryValid;

    return val;
}
