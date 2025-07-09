#pragma once
#include <map>
#include <string>
#include <vector>
#include <iostream>

#define GENERIC_EPS_SIM_SUCCESS 0
#define GENERIC_EPS_SIM_ERROR   1

    // Standalone EPS model
class EPSModel {

public:
    EPSModel(
        double initial_soc = 1.0,       // Initial battery state-of-charge
        double battery_voltage = 24.0,   // Nominal battery voltage (V)
        double max_battery_wh = 10.0,    // Battery capacity (Wh)
        double power_per_panel_w = 26.91  // Solar panel power
    );

    void step(double timestep, bool in_sun);   // Advance the simulation
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

    void update_battery_values(double timestep, bool in_sun, const std::vector<bool>& switch_states);   // Update battery and bus voltage
    Rail bus[5];   // 0=Battery, 1=3.3V, 2=5V, 3=12V, 4=Solar
    Switch switches[8];   // 8 power switches
    double power_per_panel;   // Watts generated per solar panel
    double max_battery;       // Battery capacity
    double nominal_batt_voltage;
};