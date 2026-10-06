#ifndef DASHBOARD_HPP
#define DASHBOARD_HPP

#include "Types.hpp"


class Dashboard {
public:
    Dashboard() = default;
    void clearScreen() const;
    // Display the current status of the platform
    void displayPlatformStatus(const SignalData& signals, 
                              const SignalValidation& validation, 
                              const DiagnosticReport& report) const;
};



#endif // DASHBOARD_HPP