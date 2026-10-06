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
    bool runSelfTest(SignalData& signals);
    void restart(SignalData& signals);
    void shutdown();

    // Helper to get default reference signal values
    static SignalData getReferenceSignals();

    // Core decision engine
    DiagnosticReport evaluateAndDecide(const SignalData& signals, const SignalValidation& validation);

    // Display formatted platform status matching spec requirements
    

};

#endif // CONTROL_ECU_HPP
