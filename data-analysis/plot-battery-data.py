import pandas as pd
import matplotlib.pyplot as plt
import numpy as np
from scipy.interpolate import interp1d

def plot_eps_voltage(filepath, test_desc, time_col='ElapsedTime', voltage_col='BatteryVoltage', insun_col='InSun'):

    df = pd.read_csv(filepath)
    plt.figure(figsize=(10, 5))
    plt.plot(df[time_col], df[voltage_col], marker='o', label='Battery Voltage (V)', color='tab:blue', linewidth=2)

    # Shade regions where the satellite is in the dark
    in_dark = df[insun_col] == 0
    starts = np.where((~in_dark.shift(fill_value=False)) & in_dark)[0]
    ends = np.where((in_dark.shift(fill_value=False)) & ~in_dark)[0]
    if in_dark.iloc[-1]:
        ends = np.append(ends, len(df) - 1)
    for start, end in zip(starts, ends):
        plt.axvspan(df[time_col].iloc[start], df[time_col].iloc[end], color='gray', alpha=0.2, label='Eclipse' if start == starts[0] else "")

    plt.title('Battery Voltage vs. Time')
    plt.suptitle(f"Test Scenario: {test_desc}", fontsize=12, y=0.96)
    plt.xlabel(f'{time_col} (seconds)')
    plt.ylabel(f'{voltage_col} (V)')
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.show()

def plot_eps_soc(filepath, test_desc, time_col='ElapsedTime', soc_col='SOC', insun_col='InSun'):

    df = pd.read_csv(filepath)
    plt.figure(figsize=(10, 5))
    plt.plot(df[time_col], df[soc_col], marker='s', label='SOC', color='tab:orange', linewidth=2)

    # Shade regions where the satellite is in the dark
    in_dark = df[insun_col] == 0
    starts = np.where((~in_dark.shift(fill_value=False)) & in_dark)[0]
    ends = np.where((in_dark.shift(fill_value=False)) & ~in_dark)[0]
    if in_dark.iloc[-1]:
        ends = np.append(ends, len(df) - 1)
    for start, end in zip(starts, ends):
        plt.axvspan(df[time_col].iloc[start], df[time_col].iloc[end], color='gray', alpha=0.2, label='Eclipse' if start == starts[0] else "")

    plt.title('SOC vs. Time')
    plt.suptitle(f"Test Scenario: {test_desc}", fontsize=12, y=0.96)
    plt.xlabel(f'{time_col} (seconds)')
    plt.ylabel(f'{soc_col} (fraction or %)')
    plt.grid(True)
    plt.legend()
    plt.tight_layout()
    plt.show()




if __name__ == '__main__':
    plot_eps_soc('outputs/nominal_always_sun_charging_log.csv', "Nominal Sun")
    plot_eps_soc('outputs/nominal_night_pass_charging_log.csv', "Nominal Night Pass")
    plot_eps_soc('outputs/test_switch7_log.csv', "Switch 7 On")
    plot_eps_soc('outputs/broken_solar_panel_log.csv', "Broken Solar Panel")
    plot_eps_soc('outputs/solar_degradation_log.csv', "Panel Degradation")
    plot_eps_soc('outputs/rapid_tumble_log.csv', "Rapid Tumbling")

    # plot_eps_voltage('outputs/nominal_always_sun_charging_log.csv', "Nominal Sun")
    # plot_eps_voltage('outputs/nominal_night_pass_charging_log.csv', "Nominal Night Pass")
    # plot_eps_voltage('outputs/test_switch7_log.csv', "Switch 7 On")
    # plot_eps_voltage('outputs/broken_solar_panel_log.csv', "Broken Solar Panel")
    # plot_eps_voltage('outputs/solar_degradation_log.csv', "Panel Degradation")
    # plot_eps_voltage('outputs/rapid_tumble_log.csv', "Rapid Tumbling")

    # plot_eps_soc('outputs/env3_brokenpanel_y.csv', "Broken Solar Panel - Y Biased Orbit")
    # plot_eps_soc('outputs/env3_brokenpanel_x.csv', "Broken Solar Panel - X Biased Orbit")