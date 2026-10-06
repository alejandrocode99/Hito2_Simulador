# Hito 2: ECU Gateway and Control Unit Simulation

This project implements a simulated Electronic Control Unit (ECU) platform in C++ with separated responsibilities between a **Gateway ECU** and a **Control ECU**.

## Architecture Overview
- **Gateway ECU**: Responsible for validating the 5 vehicle signals (Speed, RPM, Temperature, Throttle, Battery Voltage), checking range limits, and detecting physical implausibilities. Signal values are generated automatically by the simulator.
- **Control ECU**: Responsible for system state decisions based on valid signals. It manages a 6-state state machine (`INIT`, `SELF_TEST`, `OPERATIONAL`, `DEGRADED`, `SAFE_STATE`, `SHUTDOWN`), checks signal threshold conditions (Normal, Warning, Critical), and evaluates cross-signal coherence.

## Signal Ranges & Thresholds

| Signal | Units | Valid Range (Gateway) | Normal (Control) | Warning (Control) | Critical (Control) |
|---|---|---|---|---|---|
| Speed | km/h | 0 to 250 | Coherent | Inconsistency | Inconsistency / Fault |
| RPM | rpm | 0 to 8000 | <= 6000 rpm | > 6000 rpm | > 7000 rpm |
| Temperature | °C | -40 to 150 | < 100°C | 100°C to 110°C | > 110°C |
| Throttle | % | 0 to 100 | Coherent | Minor Inconsistency | High Inconsistency |
| Battery | V | 9 to 16 | 12V to 16V | 11V to 12V | < 11V |

## State Machine Transitions
- **`INIT`**: System initialization.
- **`SELF_TEST`**: Executed upon startup to verify sensor channels and state logic.
- **`OPERATIONAL`**: Active when all signals are normal and coherent.
- **`DEGRADED`**: Active when one or more warning conditions exist.
- **`SAFE_STATE`**: Active when any critical condition occurs (highest priority).
- **`SHUTDOWN`**: Controlled system termination.

## Compilation and Execution

### Build
To compile the project using `make`:
```bash
make
```
Or directly with `g++`:
```bash
g++ -Wall -Wextra -pedantic -std=c++17 main.cpp GatewayECU.cpp ControlECU.cpp -o ecu_simulation
```

### Run
```bash
./ecu_simulation
```

## Vehicle Simulation
The program starts the vehicle simulation automatically. It generates plausible initial signals and updates them every second. Speed, RPM, and throttle evolve together; temperature changes gradually toward operating temperature and the running battery voltage stays near its charging level. Press Enter to stop the simulation and gracefully shut down the ECU.
