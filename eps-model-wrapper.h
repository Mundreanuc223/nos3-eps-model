#pragma once
#include <map>
#include <string>
#include <vector>
#include <array>

// Standalone EPS model
class EPSModel {
public:
    EPSModel(
        double initial_soc = 1.0,        // Initial battery state-of-charge
        double battery_voltage = 24.0,   // Nominal battery voltage (V)
        double max_battery_wh = 10.0,    // Battery capacity (Wh)
    );

    void step(double timestep, const std::array<double, 3>& sun_vector);   // Advance the simulation
    void set_switch(int switch_num, bool state);   // Set an individual switch state
    double get_battery_voltage();
    double get_battery_soc();   // Get the battery's state of charge: 0.0 - 1.0
    void output_status();   // Get a snapshot of EPS info

private:
    struct Switch {
        double voltage = 0.0;
        double current = 0.0;
        bool state = false;
    };

    struct Rail {
        double voltage = 0.0;
        double current = 0.0;
        double temperature = 25.0;
        double battery_watthrs = 0.0;
    };

    void update_panel_powers_from_sunvec(const std::array<double, 3>& sv); // Helper to calculate panel powers from sun vectors
    void update_battery_values(double timestep, double total_panel_power, const std::vector<bool>& switch_states);   // Update battery and bus voltage
    bool in_sun(const std::array<double, 3>& sun_vector);
    void set_power_per_panel(const std::array<double, 5>& new_values);

    std::array<double, 5> power_per_panel; // MAXIMUM Watts per panel (solar_inputs are 5: +X, -X, +Y, -Y, -Z)
    std::array<double, 5> panel_inputs; // Actual input for each panel, updated each step
    Rail bus[5];   // 0=Battery, 1=3.3V, 2=5V, 3=12V, 4=Solar
    Switch switches[8];   // 8 power switches
    double max_battery;   // Battery capacity
    double nominal_batt_voltage;
    double battery_soc;
};