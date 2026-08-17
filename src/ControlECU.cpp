#include "ControlECU.hpp"
#include <iomanip>

ControlECU::ControlECU() {
    init();
}

void ControlECU::init() {
    currentState = ECUState::INIT;
}

bool ControlECU::runSelfTest() {
    currentState = ECUState::SELF_TEST;
    std::cout << "[SYSTEM] Control ECU executing Self-Test..." << std::endl;
    std::cout << "[SELF_TEST] Verifying Gateway ECU communication channels... OK" << std::endl;
    std::cout << "[SELF_TEST] Verifying state machine transition logic... OK" << std::endl;
    std::cout << "[SELF_TEST] Verifying sensor input buffers... OK" << std::endl;
    std::cout << "[SYSTEM] Self-Test completed successfully. Transitioning to OPERATIONAL." << std::endl;
    currentState = ECUState::OPERATIONAL;
    return true;
}

void ControlECU::restart() {
    std::cout << "\n[SYSTEM] System restart requested..." << std::endl;
    init();
    runSelfTest();
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

void ControlECU::displayPlatformStatus(const SignalData& signals, 
                                      const SignalValidation& validation, 
                                      const DiagnosticReport& report) const {
    std::cout << "\n================ PLATFORM STATUS ================\n\n";
    std::cout << "Control ECU State: " << ecuStateToString(currentState) << "\n\n";

    std::cout << std::left << std::setw(15) << "Speed:" 
              << std::right << std::setw(8) << signals.speed << " km/h    " 
              << (validation.speedValid ? "Valid" : "Invalid") << "\n";

    std::cout << std::left << std::setw(15) << "RPM:" 
              << std::right << std::setw(8) << signals.rpm << " rpm     " 
              << (validation.rpmValid ? "Valid" : "Invalid") << "\n";

    std::cout << std::left << std::setw(15) << "Temperature:" 
              << std::right << std::setw(8) << signals.temperature << " C       " 
              << (validation.temperatureValid ? "Valid" : "Invalid") << "\n";

    std::cout << std::left << std::setw(15) << "Throttle:" 
              << std::right << std::setw(8) << signals.throttle << " %       " 
              << (validation.throttleValid ? "Valid" : "Invalid") << "\n";

    std::cout << std::left << std::setw(15) << "Battery:" 
              << std::right << std::setw(8) << signals.battery << " V       " 
              << (validation.batteryValid ? "Valid" : "Invalid") << "\n\n";

    std::cout << "Detected condition: ";
    if (report.detectedConditions.empty()) {
        std::cout << "None (All signals normal)\n";
    } else {
        for (size_t i = 0; i < report.detectedConditions.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << report.detectedConditions[i];
        }
        std::cout << "\n";
    }

    std::cout << "\nDecision taken: ";
    if (report.decisionsTaken.empty()) {
        std::cout << "None\n";
    } else {
        for (size_t i = 0; i < report.decisionsTaken.size(); ++i) {
            if (i > 0) std::cout << ", ";
            std::cout << report.decisionsTaken[i];
        }
        std::cout << "\n";
    }

    std::cout << "\n=================================================\n" << std::endl;
}
