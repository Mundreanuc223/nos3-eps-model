#include "eps-model-wrapper.h"
#include "helper-functions.h"
#include <array>
#include <iostream>
#include <fstream>

/*
Nominal Scenario: Power should increase/stay the same when in the light
*/
void nominal_always_sun_charging_test() {

    // Initialize the EPS model with desired parameters
    EPSModel eps(0.20, 24.0, 20.0);

    // Pass in the sun vector data
    std::vector<std::array<double, 3>> sun_vectors = read_sun_vectors("../inputs/always_sun");

    double timestep = 10.0; // 10 second per step (change as needed, ensure it matches the time step of your solar data)
    double elapsed = 0.0;
    std::string log_file = "../outputs/nominal_always_sun_charging_log.csv";   // Set the file for output data

    // Optionally, clear old file at start (overwrite)
    std::ofstream(log_file).close();

    // Loop through all the environmental data, stepping in the specified time intervals
    for (const auto& sv : sun_vectors) {
        bool sun = eps.in_sun(sv);
        eps.step(timestep, sv);
        eps.log_battery_voltage(log_file, elapsed, sun);
        elapsed += timestep;
    }

    std::cout << "** Nominal (Always Sun) Charging Test Complete. Log written to: " << log_file << " **" << std::endl;
}

/*
Nominal Scenario: Power should increase/stay the same when in the light, drop a little when in darkness
*/
void nominal_night_pass_charging_test() {

    // Initialize the EPS model with desired parameters
    EPSModel eps(0.80, 24.0, 20.0);

    // Pass in the sun vector data
    std::vector<std::array<double, 3>> sun_vectors = read_sun_vectors("../inputs/night_pass");

    double timestep = 10.0; // 10 second per step (change as needed, ensure it matches the time step of your solar data)
    double elapsed = 0.0;
    std::string log_file = "../outputs/nominal_night_pass_charging_log.csv";   // Set the file for output data

    // Optionally, clear old file at start (overwrite)
    std::ofstream(log_file).close();

    // Loop through all the environmental data, stepping in the specified time intervals
    for (const auto& sv : sun_vectors) {
        bool sun = eps.in_sun(sv);
        eps.step(timestep, sv);
        eps.log_battery_voltage(log_file, elapsed, sun);
        elapsed += timestep;
    }

    std::cout << "** Nominal (Night Pass) Charging Test Complete. Log written to: " << log_file << " **" << std::endl;
}

/*
TDAC Test 2: Off-nominal scenario where switch 7 was stuck on / an ill command turned it on
causing it to drain battery even in sunlight
*/
void switch_7_test(){

    EPSModel eps(0.80, 24.0, 20.0);

    // Turn ON switch 7 only (rest are off by default)
    eps.set_switch(7, true);

    std::vector<std::array<double, 3>> sun_vectors = read_sun_vectors("../inputs/night_pass");

    double timestep = 10.0;
    double elapsed = 0.0;
    std::string log_file = "../outputs/test_switch7_log.csv";

    std::ofstream(log_file).close();

    for (const auto& sv : sun_vectors) {
        bool sun = eps.in_sun(sv);
        eps.step(timestep, sv);
        eps.log_battery_voltage(log_file, elapsed, sun);
        elapsed += timestep;
    }

    std::cout << "** Switch 7 Test Complete. Log written to: " << log_file << " **" << std::endl;
}

/*
TDAC Test 3: Both the X solar panels go out, resulting in much slower charging
*/
void broken_solar_panels_test(){

    EPSModel eps(0.20, 24.0, 20.0);

    // Start with all panels working
    std::array<double, 5> panel_powers = {26.91, 26.91, 26.91, 26.91, 26.91};
    eps.set_power_per_panel(panel_powers);

    // Read sun vectors from file
    std::vector<std::array<double, 3>> sun_vectors = read_sun_vectors("../inputs/always_sun");

    double timestep = 10.0; // 1 second per step (change as needed)
    double elapsed = 0.0;
    std::string log_file = "../outputs/broken_solar_panel_log.csv";

    // Optionally, clear old file at start (overwrite)
    std::ofstream(log_file).close();

    for (size_t i = 0; i < sun_vectors.size(); ++i) {
        // Break both X panels after 100 steps
        if (i == 100) {
            // 0, 1 = X+/X- are broken
            panel_powers[0] = 0;
            panel_powers[1] = 0;
            eps.set_power_per_panel(panel_powers);
        }
        const auto& sv = sun_vectors[i];
        bool sun = eps.in_sun(sv);
        eps.step(timestep, sv);
        eps.log_battery_voltage(log_file, elapsed, sun);
        elapsed += timestep;
    }

    std::cout << "** Broken Solar Panel Test Complete. Log written to: " << log_file << " **" << std::endl;
}

/*
TDAC Test 4: The satellite experiences rapid tumbling, validate how the EPS's charging is effected
*/
void rapid_tumbling_test(){

    EPSModel eps(0.20, 24.0, 20.0);

    std::array<double, 5> new_panel_powers = {26.91, 26.91, 26.91, 26.91, 26.91};
    eps.set_power_per_panel(new_panel_powers);

    // Read sun vectors from file
    std::vector<std::array<double, 3>> sun_vectors = read_sun_vectors("../inputs/rapid_tumble");

    double timestep = 10.0; // 1 second per step (change as needed)
    double elapsed = 0.0;
    std::string log_file = "../outputs/rapid_tumble_log.csv";

    // Optionally, clear old file at start (overwrite)
    std::ofstream(log_file).close();

    for (const auto& sv : sun_vectors) {
        bool sun = eps.in_sun(sv);
        eps.step(timestep, sv);
        eps.log_battery_voltage(log_file, elapsed, sun);
        elapsed += timestep;
    }

    std::cout << "** Rapid Tumble Test Complete. Log written to: " << log_file << " **" << std::endl;
}


/*
General Test 1: The solar panels have reduced power because of degradation (aging), resulting in much slower charging
*/
void solar_degradation_test() {
    EPSModel eps(0.20, 24.0, 20.0);

    // Set degraded solar panel power
    std::array<double, 5> degraded_panels = {16.15, 16.15, 16.15, 16.15, 16.15};
    eps.set_power_per_panel(degraded_panels);

    // Read sun vectors as usual
    std::vector<std::array<double, 3>> sun_vectors = read_sun_vectors("../inputs/always_sun");

    double timestep = 10.0;
    double elapsed = 0.0;
    std::string log_file = "../outputs/solar_degradation_log.csv";
    std::ofstream(log_file).close();

    for (const auto& sv : sun_vectors) {
        bool sun = eps.in_sun(sv);
        eps.step(timestep, sv);
        eps.log_battery_voltage(log_file, elapsed, sun);
        elapsed += timestep;
    }
    std::cout << "** Solar Degradation Test Complete. Log written to: " << log_file << " **" << std::endl;
}


int main() {
    nominal_always_sun_charging_test();
    nominal_night_pass_charging_test();
    switch_7_test();
    broken_solar_panels_test();
    rapid_tumbling_test();
    solar_degradation_test();
}