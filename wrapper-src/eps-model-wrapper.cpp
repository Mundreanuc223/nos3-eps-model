#include "eps-model-wrapper.h"
#include <algorithm>
#include <iomanip>
#include <iostream>
#include <fstream>

// Constructor: initialize rails, switches, parameters
EPSModel::EPSModel(double initial_soc, double battery_voltage, double max_battery_wh){

    // Initialize EPS values
    this->max_battery = max_battery_wh;
    this->nominal_batt_voltage = battery_voltage;
    this->battery_soc = initial_soc;

    // Set per panel power
    this->power_per_panel = {26.91, 26.91, 26.91, 26.91, 26.91}; // +X, -X, +Y, -Y, -Z (use real value for -Z if you have it; I used 10.5 as a placeholder)

    // Initialize the battery rail (index 0)
    bus[0].voltage = nominal_batt_voltage * 1000;  // stored in mV (to match NOS3 sim)
    bus[0].temperature = 30.0;   // Default from NOS3 config
    bus[0].battery_watthrs = max_battery * initial_soc;

    // 3.3V rail (index 1)
    bus[1].voltage = 3.3 * 1000;
    bus[1].current = 0.15 * 1000; // mA

    // 5V rail (index 2)
    bus[2].voltage = 5.0 * 1000;
    bus[2].current = 0.10 * 1000;

    // 12V rail (index 3)
    bus[3].voltage = 12.0 * 1000;
    bus[3].current = 0.05 * 1000;

    // Solar array (index 4)
    bus[4].voltage = 32.0 * 1000;
    bus[4].current = 4.0 * 1000;
    bus[4].temperature = 80.0;

    // Set switch defaults to match NOS3 config XML
    switches[0].voltage = 1.23 * 1000;
    switches[0].current = 4.56 * 1000;
    switches[1].voltage = 3.30 * 1000;
    switches[1].current = 0.25 * 1000;
    switches[2].voltage = 3.30 * 1000;
    switches[2].current = 0.25 * 1000;
    switches[3].voltage = 3.30 * 1000;
    switches[3].current = 0.25 * 1000;
    switches[4].voltage = 3.30 * 1000;
    switches[4].current = 0.25 * 1000;
    switches[5].voltage = 3.30 * 1000;
    switches[5].current = 0.25 * 1000;
    switches[6].voltage = 3.30 * 1000;
    switches[6].current = 0.25 * 1000;
    switches[7].voltage = 12.00 * 1000;
    switches[7].current = 1.23 * 1000;

    for (int i = 0; i < 8; ++i) {
        switches[i].state = false; // Off by default
    }
}

// Step function: advances simulation by timestep (sec), toggles in_sun state
void EPSModel::step(double timestep, const std::array<double, 3>& sun_vector) {

    // Update the panel powers
    update_panel_powers_from_sunvec(sun_vector);

    // Prepare vector of switch states for update_battery_values (for compatibility)
    std::vector<bool> switch_states(8);
    for (int i = 0; i < 8; ++i){
        switch_states[i] = switches[i].state;
    }

    double total_panel_power = 0.0;
    for (int i = 0; i < 5; ++i) total_panel_power += panel_inputs[i];

    update_battery_values(timestep, total_panel_power, switch_states);
}

// Set the state of a particular switch
void EPSModel::set_switch(int switch_num, bool state) {
    if (switch_num >= 0 && switch_num < 8){
        switches[switch_num].state = state;
    }
}

// Get the battery's current voltage in V
double EPSModel::get_battery_voltage() {
    return bus[0].voltage / 1000.0; // mV to V
}

// Get the battery's state of charge
double EPSModel::get_battery_soc() {
    return std::max(0.0, std::min(1.0, bus[0].battery_watthrs / max_battery));  // Clamp between 0.0 and 1.0
}

// update each particular panels power levels based off the sun vector (logic from nos3)
void EPSModel::update_panel_powers_from_sunvec(const std::array<double, 3>& sun_vec) {
    panel_inputs[0] = (sun_vec[0] > 0) ? sun_vec[0] * power_per_panel[0] : 0.0;   // +X
    panel_inputs[1] = (sun_vec[0] < 0) ? -sun_vec[0] * power_per_panel[1] : 0.0;  // -X
    panel_inputs[2] = (sun_vec[1] > 0) ? sun_vec[1] * power_per_panel[2] : 0.0;   // +Y
    panel_inputs[3] = (sun_vec[1] < 0) ? -sun_vec[1] * power_per_panel[3] : 0.0;  // -Y
    panel_inputs[4] = (sun_vec[2] < 0) ? -sun_vec[2] * power_per_panel[4] : 0.0;  // -Z
}

void EPSModel::output_status() {
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "=== EPS STATUS ===\n";
    std::cout << "Battery Voltage: " << bus[0].voltage / 1000.0 << " V\n";
    std::cout << "Battery SOC: " << 100 * (bus[0].battery_watthrs / max_battery) << " %\n";
    std::cout << "Battery Energy: " << bus[0].battery_watthrs << " Wh\n";
    std::cout << "Solar Voltage: " << bus[4].voltage / 1000.0 << " V\n";
    std::cout << "Solar Current: " << bus[4].current / 1000.0 << " A\n";
    std::cout << "Switch States: ";
    for (int i = 0; i < 8; ++i) {
        std::cout << (switches[i].state ? "[ON]" : "[OFF]") << " ";
    }
    std::cout << "\n";
    std::cout << "===================\n";
}

void EPSModel::log_battery_voltage(const std::string& filename, double elapsed_time, bool in_sun){
    std::ofstream ofs(filename, std::ios::app); // append to preserve all steps

    // If file is empty, write the header first
    if (ofs.tellp() == 0) {
        ofs << "ElapsedTime,SOC,BatteryVoltage,InSun\n";
    }

    ofs << std::fixed << std::setprecision(2)
        << elapsed_time << ","
        << std::setprecision(4) << battery_soc << ","
        << (bus[0].voltage / 1000.0) << ","
        << (in_sun ? 1 : 0) << "\n";
}

// Returns if the sat is in the sun at all
bool EPSModel::in_sun(const std::array<double, 3>& sun_vector){
    return sun_vector[0] != 0.0 || sun_vector[1] != 0.0 || sun_vector[2] != 0.0;
}

// Set the powers per panel
void EPSModel::set_power_per_panel(const std::array<double, 5>& new_values) {
    power_per_panel = new_values;
}

// Core NOS3 logic: update battery charge/voltage each timestep
void EPSModel::update_battery_values(double timestep, double total_panel_power, const std::vector<bool>& switch_states)
{
    // Power OUT: loads on rails + switches
    double p_out = 0.0;
    for (int i = 1; i < 4; ++i)
        p_out += (bus[i].voltage / 1000.0) * (bus[i].current / 1000.0); // P = V x I
    for (int i = 0; i < 8; ++i)
        if (switch_states[i])
            p_out += (switches[i].voltage / 1000.0) * (switches[i].current / 1000.0);

    // Power IN: only from solar if in_sun
    double p_in = total_panel_power;

    // Change in Wh this step
    double delta_p = timestep * (p_in - p_out);
    bus[0].battery_watthrs += delta_p / 3600.0; // timestep is in seconds

    // Clamp to [0, max_battery]
    if (bus[0].battery_watthrs > max_battery) bus[0].battery_watthrs = max_battery;
    if (bus[0].battery_watthrs < 0.0) bus[0].battery_watthrs = 0.0;

    // Battery voltage: linear function of SOC, as in the NOS3 sim
    double batt_min_v = 0.95 * nominal_batt_voltage;
    double batt_diff = 0.1 * nominal_batt_voltage;
    bus[0].voltage = 1000.0 * (batt_min_v + batt_diff * (bus[0].battery_watthrs / max_battery));
    battery_soc = std::max(0.0, std::min(1.0, bus[0].battery_watthrs / max_battery));
}
