#ifndef SIMULATOR_HPP
#define SIMULATOR_HPP

#include "ControlECU.hpp"
#include "Dashboard.hpp"
#include "GatewayECU.hpp"

class Simulator {
private:
	GatewayECU gateway;
	ControlECU control;
	Dashboard dashboard;
	SignalData currentSignals;
	SignalValidation currentValidation;
	DiagnosticReport currentReport;

	static double generateRandomNumber(double minimum, double maximum);
	static SignalData generateInitialSignals();
	static SignalData updateSimulation(const SignalData& current);
	void updateCurrentStatus();
	void runContinuousSimulation();

public:
	void run();
};

#endif // SIMULATOR_HPP
