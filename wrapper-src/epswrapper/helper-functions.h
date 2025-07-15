#pragma once
#include <vector>
#include <array>
#include <string>
#include <fstream>
#include <sstream>
#include <stdexcept>

inline std::vector<std::array<double, 3>> read_sun_vectors(const std::string& filename) {
    std::vector<std::array<double, 3>> sun_vectors;
    std::ifstream infile(filename);
    if (!infile) {
        throw std::runtime_error("Could not open sun vector file: " + filename);
    }
    std::string line;
    while (std::getline(infile, line)) {
        if (line.empty()) continue;
        std::istringstream ss(line);
        std::string item;
        std::array<double, 3> vec = {0.0, 0.0, 0.0};
        int idx = 0;
        while (std::getline(ss, item, ',') && idx < 3) {
            vec[idx++] = std::stod(item);
        }
        if (idx == 3) sun_vectors.push_back(vec);
    }
    return sun_vectors;
}