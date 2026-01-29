/**
 * @file math_operations.cpp
 * @brief Implementation of mathematical operations
 */

#include "math_operations.hpp"
#include <algorithm>
#include <numeric>

namespace MathOps {

// Calculator implementation
Calculator::Calculator() : precision_(6) {}

Calculator::~Calculator() {}

double Calculator::add(double a, double b) const {
    return a + b;
}

double Calculator::subtract(double a, double b) const {
    return a - b;
}

double Calculator::multiply(double a, double b) const {
    return a * b;
}

double Calculator::divide(double a, double b) const {
    if (b == 0.0) {
        throw std::invalid_argument("Division by zero");
    }
    return a / b;
}

double Calculator::power(double base, double exponent) const {
    return std::pow(base, exponent);
}

double Calculator::squareRoot(double value) const {
    if (value < 0.0) {
        throw std::invalid_argument("Cannot calculate square root of negative number");
    }
    return std::sqrt(value);
}

unsigned long long Calculator::factorial(int n) const {
    if (n < 0) {
        throw std::invalid_argument("Factorial is not defined for negative numbers");
    }
    if (n == 0 || n == 1) {
        return 1;
    }
    unsigned long long result = 1;
    for (int i = 2; i <= n; ++i) {
        result *= i;
    }
    return result;
}

bool Calculator::validateInput(double value) const {
    return std::isfinite(value);
}

// Statistics implementation
double Statistics::mean(const std::vector<double>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Cannot calculate mean of empty dataset");
    }
    return std::accumulate(data.begin(), data.end(), 0.0) / data.size();
}

double Statistics::median(std::vector<double> data) {
    if (data.empty()) {
        throw std::invalid_argument("Cannot calculate median of empty dataset");
    }
    std::sort(data.begin(), data.end());
    size_t n = data.size();
    if (n % 2 == 0) {
        return (data[n/2 - 1] + data[n/2]) / 2.0;
    } else {
        return data[n/2];
    }
}

double Statistics::standardDeviation(const std::vector<double>& data, bool sample) {
    if (data.empty()) {
        throw std::invalid_argument("Cannot calculate standard deviation of empty dataset");
    }
    if (sample && data.size() == 1) {
        throw std::invalid_argument("Sample standard deviation requires at least 2 data points");
    }
    return std::sqrt(variance(data, sample));
}

double Statistics::variance(const std::vector<double>& data, bool sample) {
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

double Statistics::min(const std::vector<double>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Cannot find minimum of empty dataset");
    }
    return *std::min_element(data.begin(), data.end());
}

double Statistics::max(const std::vector<double>& data) {
    if (data.empty()) {
        throw std::invalid_argument("Cannot find maximum of empty dataset");
    }
    return *std::max_element(data.begin(), data.end());
}

double Statistics::range(const std::vector<double>& data) {
    return max(data) - min(data);
}

const char* mathErrorToString(MathError error) {
    switch (error) {
        case MathError::NONE: return "No error";
        case MathError::DIVISION_BY_ZERO: return "Division by zero";
        case MathError::INVALID_INPUT: return "Invalid input";
        case MathError::OVERFLOW: return "Numerical overflow";
        case MathError::UNDERFLOW: return "Numerical underflow";
        case MathError::DOMAIN_ERROR: return "Domain error";
        default: return "Unknown error";
    }
}

} // namespace MathOps
