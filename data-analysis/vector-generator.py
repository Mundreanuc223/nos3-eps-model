import numpy as np
import math

def night_pass_generator():

    period = 5400         # Orbit period in seconds (90 minutes)
    dt = 10               # time step in seconds
    steps = period // dt

    with open("night_pass", "w") as f:
        for i in range(int(steps)):
            t = i * dt
            # Phase (in radians), rotate full 360° per orbit
            phase = 2 * np.pi * t / period
            # Let's say the Sun moves in the XY plane (projected onto body axes)
            x = np.cos(phase)
            y = np.sin(phase)
            z = 0
            # Simulate eclipse for 40% of the orbit
            if (t % period) > (0.6 * period):
                x = 0
                y = 0
                z = 0
            f.write(f"{x:.4f},{y:.4f},{z:.4f}\n")

def nominal_sunlight():
    # Simulation parameters
    orbit_period = 5400  # seconds in one orbit (~100 min)
    timestep = 10  # seconds per step
    n_steps = int(orbit_period / timestep)

    # Output file
    filename = "always_sun"

    with open(filename, "w") as f:
        for i in range(n_steps):
            t = i * timestep
            # Sun vector rotates in XY-plane
            angle = 2 * math.pi * t / orbit_period
            x = round(math.cos(angle), 3)
            y = round(math.sin(angle), 3)
            z = 0.0  # Assuming 2D for simplicity
            f.write(f"{x},{y},{z}\n")

    print(f"File '{filename}' written with {n_steps} steps of continuous sunlight.")

def rapid_tumble_generator():
    period = 5400         # Orbit period in seconds (90 minutes)
    dt = 10               # time step in seconds
    steps = period // dt

    with open("inputs/rapid_tumble", "w") as f:
        for i in range(int(steps)):
            t = i * dt
            # Rapid, pseudo-random tumbling (multiple harmonics)
            phase1 = 2 * np.pi * t / period           # Full orbit
            phase2 = 10 * phase1                      # 10x faster “tumble”
            phase3 = 17 * phase1                      # 17x for non-periodicity

            # Mix axes, so it isn’t just rotating smoothly
            x = 0.45 * np.cos(phase2) + 0.35 * np.sin(phase3) + 0.25 * np.cos(phase1)
            y = 0.45 * np.sin(phase2) + 0.35 * np.cos(phase3) + 0.25 * np.sin(phase1)
            z = 0.2 * np.cos(phase2 + phase3)

            # Normalize, but always keep at least 0.7 “exposure”
            mag = np.sqrt(x**2 + y**2 + z**2)
            scale = max(0.7, min(1.0, mag))
            x *= 0.8 / scale
            y *= 0.8 / scale
            z *= 0.8 / scale

            f.write(f"{x:.4f},{y:.4f},{z:.4f}\n")

if __name__ == "__main__":
    night_pass_generator()
    nominal_sunlight()
    rapid_tumble_generator()