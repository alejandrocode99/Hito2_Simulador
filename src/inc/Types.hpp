#ifndef TYPES_HPP
#define TYPES_HPP

#include <string>
#include <vector>

// 6 states state machine for ECU
enum class ECUState {
    INIT,
    SELF_TEST,
    OPERATIONAL,
    DEGRADED,
    SAFE_STATE,
    SHUTDOWN
};

inline std::string ecuStateToString(ECUState state) {
    switch (state) {
        case ECUState::INIT:        return "INIT";
        case ECUState::SELF_TEST:   return "SELF_TEST";
        case ECUState::OPERATIONAL: return "OPERATIONAL";
        case ECUState::DEGRADED:    return "DEGRADED";
        case ECUState::SAFE_STATE:  return "SAFE_STATE";
        case ECUState::SHUTDOWN:    return "SHUTDOWN";
    }
    return "UNKNOWN";
}

// Struct representing the 5 vehicle input signals (initialized to reference values)
struct SignalData {
    double speed{0.0};         // km/h (range: 0 to 250) - Ref: 0 km/h
    int rpm{2000};             // rpm  (range: 0 to 8000) - Ref: 2000 rpm
    double temperature{50.0};  // C    (range: -40 to 150) - Ref: 50 C
    double throttle{0.0};      // %    (range: 0 to 100) - Ref: 0 %
    double battery{12.0};      // V    (range: 9 to 16)  - Ref: 12 V
};

// Gateway validation result for individual signals and overall signal integrity
struct SignalValidation {
    bool speedValid{false};
    bool rpmValid{false};
    bool temperatureValid{false};
    bool throttleValid{false};
    bool batteryValid{false};
    bool allValid{false};
    std::vector<std::string> validationErrors;
};

enum class ConditionLevel {
    NORMAL,
    WARNING,
    CRITICAL
};

// Evaluation result produced by Control ECU
struct DiagnosticReport {
    ConditionLevel overallCondition{ConditionLevel::NORMAL};
    ECUState resultingState{ECUState::OPERATIONAL};
    std::vector<std::string> detectedConditions;
    std::vector<std::string> decisionsTaken;
};

#endif // TYPES_HPP
