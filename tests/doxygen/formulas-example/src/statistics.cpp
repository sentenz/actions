/**
 * @file statistics.cpp
 * @brief Implementation of statistical functions
 */

#include "statistics.hpp"
#include <algorithm>
#include <numeric>
#include <cmath>
#include <stdexcept>

namespace Stats {

double mean(const std::vector<double>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Cannot calculate mean of empty dataset");
    }
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

double variance(const std::vector<double>& data, bool sample) {
    if (data.empty()) {
        throw std::invalid_argument("Cannot calculate variance of empty dataset");
    }
    
    double mean_val = mean(data);
    double sum_squared_diff = 0.0;
    
    for (double value : data) {
        double diff = value - mean_val;
        sum_squared_diff += diff * diff;
    }
    
    size_t divisor = sample ? data.size() - 1 : data.size();
    return sum_squared_diff / divisor;
}

double standardDeviation(const std::vector<double>& data, bool sample) {
    return std::sqrt(variance(data, sample));
}

std::vector<double> quadraticRoots(double a, double b, double c) {
    std::vector<double> roots;
    
    if (a == 0.0) {
        // Linear equation: bx + c = 0
        if (b != 0.0) {
            roots.push_back(-c / b);
        }
        return roots;
    }
    
    // Calculate discriminant
    double discriminant = b * b - 4 * a * c;
    
    if (discriminant > 0.0) {
        // Two distinct real roots
        double sqrt_disc = std::sqrt(discriminant);
        roots.push_back((-b + sqrt_disc) / (2 * a));
        roots.push_back((-b - sqrt_disc) / (2 * a));
    } else if (discriminant == 0.0) {
        // One repeated root
        roots.push_back(-b / (2 * a));
    }
    // If discriminant < 0, no real roots (return empty vector)
    
    return roots;
}

} // namespace Stats
