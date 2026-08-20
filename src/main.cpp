#include <iostream>
#include <limits>
#include <algorithm>
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

    SignalData currentSignals;

    // Execute Self-Test sequence at system startup (initializes signals to reference values)
    control.runSelfTest(currentSignals);

    SignalValidation currentValidation = gateway.validateSignals(currentSignals);
    DiagnosticReport currentReport = control.evaluateAndDecide(currentSignals, currentValidation);

    bool running = true;
    while (running) {
        std::cout << "\nSuggested Menu:\n";
std::cout << "1. Input signal values\n";
        std::cout << "2. Simulate system behavior (could get out of bounds)\n";
        std::cout << "3. Show current system state\n";
        std::cout << "4. Restart system\n";
        std::cout << "5. Exit\n";
        std::cout << "Select option [1-5]: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "[ERROR] Invalid choice. Please enter a number between 1 and 5.\n";
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

                std::cout << "\n--- Simulating System Behavior (Random Signals) ---\n";
                    //for (int step = 1; step <= 5; ++step) {
                    //std::cout << "\n>>> [SIMULATION STEP " << step << "/5] <<<\n";

                    // 1. Generar señales aleatorias desde el Gateway
                    currentSignals = gateway.generateRandomSignals();

                    // 2. Validar plausibilidad física con las reglas del Gateway
                    currentValidation = gateway.validateSignals(currentSignals);

                     // 3. Evaluar reglas de diagnóstico en la ECU de Control
                    currentReport = control.evaluateAndDecide(currentSignals, currentValidation);

                    // 4. Desplegar resultados
                    control.displayPlatformStatus(currentSignals, currentValidation, currentReport);
                //}
                break;

            }
            
            case 3: {
                control.displayPlatformStatus(currentSignals, currentValidation, currentReport);
                break;
            }
            case 4: {
                control.restart(currentSignals);
                currentValidation = gateway.validateSignals(currentSignals);
                currentReport = control.evaluateAndDecide(currentSignals, currentValidation);
                break;
            }
            case 5: {
                control.shutdown();
                running = false;
                break;
            }
            default: {
                std::cout << "[ERROR] Invalid option selected. Please choose 1, 2, 3, 4 or 5.\n";
                break;
            }
        }
    }

    std::cout << "[SYSTEM] Exiting ECU simulation program. Goodbye.\n";
    return 0;
}
