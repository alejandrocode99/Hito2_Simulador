#ifndef CONTROL_ECU_HPP
#define CONTROL_ECU_HPP

#include "Types.hpp"
#include <iostream>

class ControlECU {
private:
    ECUState currentState{ECUState::INIT};

public:
    ControlECU();

    // Lifecycle methods
    void init();
    bool runSelfTest();
    void restart();
    void shutdown();

    // Core decision engine
    DiagnosticReport evaluateAndDecide(const SignalData& signals, const SignalValidation& validation);

    // Display formatted platform status matching spec requirements
    void displayPlatformStatus(const SignalData& signals, 
                              const SignalValidation& validation, 
                              const DiagnosticReport& report) const;

    ECUState getState() const { return currentState; }
    std::string getStateString() const { return ecuStateToString(currentState); }
};

#endif // CONTROL_ECU_HPP
