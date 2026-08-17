#include <iostream>
#include <limits>
#include "GatewayECU.hpp"
#include "ControlECU.hpp"

// Helper function to robustly read a double value from std::cin
double readDouble(const std::string& prompt) {
    double val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            return val;
        } else {
            std::cout << "[ERROR] Invalid input. Please enter a numerical value.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

// Helper function to robustly read an int value from std::cin
int readInt(const std::string& prompt) {
    int val;
    while (true) {
        std::cout << prompt;
        if (std::cin >> val) {
            return val;
        } else {
            std::cout << "[ERROR] Invalid input. Please enter an integer value.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        }
    }
}

int main() {
    std::cout << "=================================================\n";
    std::cout << "      SIMULATED ECU PLATFORM (GATEWAY & CONTROL) \n";
    std::cout << "=================================================\n\n";

    GatewayECU gateway;
    ControlECU control;

    // Execute Self-Test sequence at system startup
    control.runSelfTest();

    SignalData currentSignals;
    SignalValidation currentValidation = gateway.validateSignals(currentSignals);
    DiagnosticReport currentReport = control.evaluateAndDecide(currentSignals, currentValidation);

    bool running = true;
    while (running) {
        std::cout << "\nSuggested Menu:\n";
        std::cout << "1. Input signal values\n";
        std::cout << "2. Show current system state\n";
        std::cout << "3. Restart system\n";
        std::cout << "4. Exit\n";
        std::cout << "Select option [1-4]: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "[ERROR] Invalid choice. Please enter a number between 1 and 4.\n";
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            continue;
        }

        switch (choice) {
            case 1: {
                std::cout << "\n--- Signal Entry ---\n";
                currentSignals.speed = readDouble("Input speed km/h: ");
                currentSignals.rpm = readInt("Input RPM: ");
                currentSignals.temperature = readDouble("Input Temperature C: ");
                currentSignals.throttle = readDouble("Input Throttle %: ");
                currentSignals.battery = readDouble("Input Battery Voltage V: ");

                // Gateway validation
                currentValidation = gateway.validateSignals(currentSignals);

                // Control ECU evaluation
                currentReport = control.evaluateAndDecide(currentSignals, currentValidation);

                // Output platform status
                control.displayPlatformStatus(currentSignals, currentValidation, currentReport);
                break;
            }
            case 2: {
                control.displayPlatformStatus(currentSignals, currentValidation, currentReport);
                break;
            }
            case 3: {
                control.restart();
                currentValidation = gateway.validateSignals(currentSignals);
                currentReport = control.evaluateAndDecide(currentSignals, currentValidation);
                break;
            }
            case 4: {
                control.shutdown();
                running = false;
                break;
            }
            default: {
                std::cout << "[ERROR] Invalid option selected. Please choose 1, 2, 3, or 4.\n";
                break;
            }
        }
    }

    std::cout << "[SYSTEM] Exiting ECU simulation program. Goodbye.\n";
    return 0;
}
