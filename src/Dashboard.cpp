#include "Dashboard.hpp"
#include <iostream>
#include <iomanip>

void Dashboard::clearScreen() const {
    std::cout << "\033[2J\033[H" << std::flush;
}

void Dashboard::displayPlatformStatus(const SignalData& signals, 
                                      const SignalValidation& validation, 
                                      const DiagnosticReport& report) const {
    std::cout << "\n================ PLATFORM STATUS ================\n\n";
    std::cout << "Control ECU State: " << ecuStateToString(report.resultingState) << "\n\n";

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
