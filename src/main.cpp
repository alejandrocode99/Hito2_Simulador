#include <iostream>
#include <limits>
#include <algorithm>
#include <thread>
#include <chrono>
#include <future>
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
        std::cout << "3. Simulate system behavior (within bounds)\n";
        std::cout << "4. Show current system state\n";
        std::cout << "5. Restart system\n";
        std::cout << "6. Exit\n";
        std::cout << "Select option [1-6]: ";

        int choice;
        if (!(std::cin >> choice)) {
            std::cout << "[ERROR] Invalid choice. Please enter a number between 1 and 6.\n";
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

                // clear any residual characters in the input buffer
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                // start an asynchronous listener to stop the simulation when Enter is pressed
                auto inputFuture = std::async(std::launch::async, []() {
                    std::cin.get();
                });

                int step = 1;
                while (inputFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
                    // clear the screen on each refresh (ANSI Escape Code)
                    std::cout << "\033[2J\033[1;1H";

                    std::cout << "=================================================\n";
                    std::cout << "  RUNNING CONTINUOUS SYSTEM BEHAVIOR SIMULATION  \n";
                    std::cout << "  [Simulation #: " << step++ << "] - PRESS [ENTER] TO STOP...\n";
                    std::cout << "=================================================\n";

                    //Generate random signals (could be out of bounds)
                    currentSignals = gateway.generateRandomSignals();
                    currentValidation = gateway.validateSignals(currentSignals);
                    currentReport = control.evaluateAndDecide(currentSignals, currentValidation);

                    // Display the current platform status
                    control.displayPlatformStatus(currentSignals, currentValidation, currentReport);

                    // 1 second pause between readings
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                std::cout << "\n--- Simulation Stopped by User ---\n";
                break;
            }
 
            case 3: {
                // clear any residual characters in the input buffer
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');

                // start an asynchronous listener to stop the simulation when Enter is pressed
                auto inputFuture = std::async(std::launch::async, []() {
                    std::cin.get();
                });

                int step = 1;
                while (inputFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
                    // clear the screen on each refresh (ANSI Escape Code)
                    std::cout << "\033[2J\033[1;1H";

                    std::cout << "=================================================\n";
                    std::cout << "  SIMULATION: WITHIN BOUNDS (NORMAL OPERATION)   \n";
                    std::cout << "  [Step: " << step++ << "] - Press [ENTER] to stop...\n";
                    std::cout << "=================================================\n";

                    // Generate normal signals within safe ranges
                    currentSignals = gateway.generateNormalSignals();
                    currentValidation = gateway.validateSignals(currentSignals);
                    currentReport = control.evaluateAndDecide(currentSignals, currentValidation);

                    // Display the current platform status
                    control.displayPlatformStatus(currentSignals, currentValidation, currentReport);

                    // 1 second pause between readings
                    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
                }

                std::cout << "\n--- Simulation Stopped by User ---\n";
                break;
            }

            case 4: {
                control.displayPlatformStatus(currentSignals, currentValidation, currentReport);
                break;
            }
            case 5: {
                control.restart(currentSignals);
                currentValidation = gateway.validateSignals(currentSignals);
                currentReport = control.evaluateAndDecide(currentSignals, currentValidation);
                break;
            }
            case 6: {
                control.shutdown();
                running = false;
                break;
            }

            default: {
                std::cout << "[ERROR] Invalid option selected. Please choose 1, 2, 3, 4, 5 or 6.\n";
                break;
            }
        }
    }

    std::cout << "[SYSTEM] Exiting ECU simulation program. Goodbye.\n";
    return 0;
}
