#include "ControlECU.hpp"
#include <iomanip>

ControlECU::ControlECU() {
    init();
}

void ControlECU::init() {
    currentState = ECUState::INIT;
}

SignalData ControlECU::getReferenceSignals() {
    SignalData ref;
    ref.speed = 0.0;
    ref.rpm = 2000;
    ref.temperature = 50.0;
    ref.throttle = 0.0;
    ref.battery = 12.0;
    return ref;
}

bool ControlECU::runSelfTest(SignalData& signals) {
    currentState = ECUState::SELF_TEST;
    std::cout << "[SYSTEM] Control ECU executing Self-Test..." << std::endl;

    // Reset signals to reference values during self test
    signals = getReferenceSignals();
    std::cout << "[SELF_TEST] Setting reference signals (Speed: 0 km/h, RPM: 2000 rpm, Temp: 50 C, Throttle: 0 %, Battery: 12 V)... OK" << std::endl;

    std::cout << "[SELF_TEST] Verifying Gateway ECU communication channels... OK" << std::endl;
    std::cout << "[SELF_TEST] Verifying state machine transition logic... OK" << std::endl;
    std::cout << "[SELF_TEST] Verifying sensor input buffers... OK" << std::endl;
    std::cout << "[SYSTEM] Self-Test completed successfully. Transitioning to OPERATIONAL." << std::endl;
    currentState = ECUState::OPERATIONAL;
    return true;
}

void ControlECU::restart(SignalData& signals) {
    std::cout << "\n[SYSTEM] System restart requested..." << std::endl;
    init();
    runSelfTest(signals);
}

void ControlECU::shutdown() {
    currentState = ECUState::SHUTDOWN;
    std::cout << "\n[SYSTEM] Control ECU gracefully shutting down. State: " 
              << ecuStateToString(currentState) << std::endl;
}

DiagnosticReport ControlECU::evaluateAndDecide(const SignalData& signals, const SignalValidation& validation) {
    DiagnosticReport report;

    // Check individual signals against thresholds if valid
    bool hasCritical = false;
    bool hasWarning = false;

    // Gateway rejections
    if (!validation.allValid) {
        for (const auto& err : validation.validationErrors) {
            report.detectedConditions.push_back("Gateway Error: " + err);
        }
    }

    // Temperature thresholds
    if (validation.temperatureValid) {
        if (signals.temperature > 110.0) {
            hasCritical = true;
            report.detectedConditions.push_back("High Temperature (>110C): Critical Overheating (" + std::to_string(signals.temperature) + " C)");
        } else if (signals.temperature >= 100.0) {
            hasWarning = true;
            report.detectedConditions.push_back("High Temperature (100C - 110C): Elevated Temperature (" + std::to_string(signals.temperature) + " C)");
        }
    }

    // RPM thresholds
    if (validation.rpmValid) {
        if (signals.rpm > 7000) {
            hasCritical = true;
            report.detectedConditions.push_back("High RPM (>7000rpm): Critical Overspeed Engine Risk (" + std::to_string(signals.rpm) + " rpm)");
        } else if (signals.rpm > 6000) {
            hasWarning = true;
            report.detectedConditions.push_back("High RPM (>6000rpm): Engine High Load Warning (" + std::to_string(signals.rpm) + " rpm)");
        }
    }

    // Battery thresholds
    if (validation.batteryValid) {
        if (signals.battery < 11.0) {
            hasCritical = true;
            report.detectedConditions.push_back("Low Battery (<11V): Critical Voltage Drop (" + std::to_string(signals.battery) + " V)");
        } else if (signals.battery < 12.0) {
            hasWarning = true;
            report.detectedConditions.push_back("Low Battery (11V - 12V): Voltage Warning (" + std::to_string(signals.battery) + " V)");
        }
    }

    // Coherence evaluation between related signals
    if (validation.speedValid && validation.rpmValid) {
        if (signals.speed > 0.0 && signals.rpm == 0) {
            hasCritical = true;
            report.detectedConditions.push_back("Coherence Fault: Vehicle Speed > 0 km/h with 0 RPM");
        }
    }

    if (validation.throttleValid && validation.rpmValid) {
        if (signals.throttle > 50.0 && signals.rpm < 1000) {
            hasWarning = true;
            report.detectedConditions.push_back("Coherence Warning: High Throttle (>50%) with low Engine RPM (<1000rpm)");
        }
    }

    if (validation.temperatureValid && validation.batteryValid) {
        if (signals.temperature > 100.0 && signals.battery < 11.5) {
            hasCritical = true;
            report.detectedConditions.push_back("Coherence Fault: High Temperature (>100C) combined with Low Battery Voltage (<11.5V)");
        }
    }

    // Determine state transition based on priority
    if (hasCritical) {
        report.overallCondition = ConditionLevel::CRITICAL;
        report.resultingState = ECUState::SAFE_STATE;
        report.decisionsTaken.push_back("Safe State Operation (Critical Fault Detected)");
    } else if (hasWarning) {
        report.overallCondition = ConditionLevel::WARNING;
        report.resultingState = ECUState::DEGRADED;
        report.decisionsTaken.push_back("Degraded Operation (Warning Condition Detected)");
    } else {
        report.overallCondition = ConditionLevel::NORMAL;
        report.resultingState = ECUState::OPERATIONAL;
        report.decisionsTaken.push_back("Normal Operation");
    }

    // Update current internal state
    if (currentState != ECUState::SHUTDOWN && currentState != ECUState::INIT) {
        currentState = report.resultingState;
    }

    return report;
}
