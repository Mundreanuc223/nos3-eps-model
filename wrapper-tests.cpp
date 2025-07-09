#include "eps-model-wrapper.h"
#include "helper-functions.h"
#include <array>
#include <iostream>
#include <fstream>

/*
Test 1: Off-nominal scenario where switch 7 was stuck on / an ill command turned it on
 causing it to drain battery even in sunlight
*/
void switch_7_test(){

    // Initialize model at 65% charge
    EPSModel eps(0.65, 24.0, 10.0);

    // Turn ON only switch 7 (rest are off by default)
    eps.set_switch(7, true);

    // Read sun vectors from file
    std::vector<std::array<double, 3>> sun_vectors = read_sun_vectors("inputs/sun_vectors.txt");

    double timestep = 1.0; // 1 second per step (change as needed)
    double elapsed = 0.0;
    std::string log_file = "test_switch7_log.csv";

    // Optionally, clear old file at start (overwrite)
    std::ofstream(log_file).close();

    for (const auto& sv : sun_vectors) {
        bool sun = eps.in_sun(sv);
        eps.step(timestep, sv);
        eps.log_to_csv(log_file, elapsed, sun);
        elapsed += timestep;
    }

    std::cout << "Test complete. Log written to: " << log_file << std::endl;
    return 0;
}


int main() {
    switch_7_test();
}