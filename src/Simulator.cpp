#include "Simulator.hpp"

#include <algorithm>
#include <chrono>
#include <future>
#include <iostream>
#include <random>
#include <thread>

namespace {
std::mt19937 generator(std::random_device{}());
}

double Simulator::generateRandomNumber(double minimum, double maximum) {
	std::uniform_real_distribution<double> distribution(minimum, maximum);
	return distribution(generator);
}

SignalData Simulator::generateInitialSignals() {
	SignalData signals;
	signals.speed = generateRandomNumber(0.0, 80.0);
	signals.throttle = generateRandomNumber(10.0, 35.0);
	signals.rpm = 800 + static_cast<int>(signals.speed * 28.0 + signals.throttle * 8.0);
	signals.temperature = generateRandomNumber(70.0, 88.0);
	signals.battery = generateRandomNumber(13.7, 13.9);
	return signals;
}

SignalData Simulator::updateSimulation(const SignalData& current) {
	SignalData updated = current;

	const double maximumThrottle = current.speed < 100.0 ? 65.0
		                         : current.speed < 150.0 ? 40.0
		                                                 : 25.0;
	updated.throttle = std::clamp(
		current.throttle + generateRandomNumber(-6.0, 6.0), 0.0, maximumThrottle);
	if (current.speed < 1.0) {
		updated.throttle = std::max(updated.throttle, 15.0);
	}

	updated.speed = std::clamp(
		current.speed + (updated.throttle - 12.0) * 0.04
		    + generateRandomNumber(-0.5, 0.5),
		0.0, 180.0);
	updated.rpm = 800 + static_cast<int>(updated.speed * 28.0 + updated.throttle * 8.0);

	const double targetTemperature = 88.0 + updated.throttle * 0.05;
	updated.temperature = std::clamp(
		current.temperature + (targetTemperature - current.temperature) * 0.02
		    + generateRandomNumber(-0.08, 0.08),
		20.0, 110.0);
	updated.battery = std::clamp(
		current.battery + (13.8 - current.battery) * 0.1
		    + generateRandomNumber(-0.03, 0.03),
		13.5, 14.2);

	return updated;
}

void Simulator::updateCurrentStatus() {
	currentValidation = GatewayECU::validateSignals(currentSignals);
	currentReport = control.evaluateAndDecide(currentSignals, currentValidation);
}

void Simulator::runContinuousSimulation() {
	auto inputFuture = std::async(std::launch::async, []() {
		std::cin.get();
	});

	int step = 1;
	while (inputFuture.wait_for(std::chrono::seconds(0)) != std::future_status::ready) {
		dashboard.clearScreen();
		std::cout << "=================================================\n";
		std::cout << "  REALISTIC VEHICLE SIMULATION (AUTOMATIC DATA)  \n";
		std::cout << "  [Step: " << step++ << "] - Press [ENTER] to stop...\n";
		std::cout << "=================================================\n";

		currentSignals = updateSimulation(currentSignals);
		updateCurrentStatus();
		dashboard.displayPlatformStatus(currentSignals, currentValidation, currentReport);
		std::this_thread::sleep_for(std::chrono::milliseconds(1000));
	}

	std::cout << "\n--- Simulation Stopped by User ---\n";
	control.shutdown();
}

void Simulator::run() {
	std::cout << "=================================================\n";
	std::cout << "      SIMULATED ECU PLATFORM (GATEWAY & CONTROL) \n";
	std::cout << "=================================================\n\n";

	control.runSelfTest(currentSignals);
	currentSignals = generateInitialSignals();
	updateCurrentStatus();

	runContinuousSimulation();
}
