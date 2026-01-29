/**
 * @file math_operations.hpp
 * @brief Mathematical operations library
 * @author Example Team
 * @version 1.0.0
 * @date 2024-01-29
 * 
 * This file provides a comprehensive set of mathematical operations
 * including basic arithmetic, advanced calculations, and statistical functions.
 */

#ifndef MATH_OPERATIONS_HPP
#define MATH_OPERATIONS_HPP

#include <vector>
#include <stdexcept>
#include <cmath>

/**
 * @namespace MathOps
 * @brief Namespace containing all mathematical operations
 */
namespace MathOps {

/**
 * @class Calculator
 * @brief A calculator class for performing various mathematical operations
 * 
 * This class provides methods for basic arithmetic operations,
 * advanced mathematical functions, and statistical calculations.
 * 
 * @code
 * Calculator calc;
 * double result = calc.add(5.0, 3.0);
 * std::cout << "5 + 3 = " << result << std::endl;
 * @endcode
 */
class Calculator {
public:
    /**
     * @brief Default constructor
     * 
     * Initializes the calculator with default settings.
     */
    Calculator();

    /**
     * @brief Destructor
     */
    ~Calculator();

    /**
     * @brief Add two numbers
     * @param a First operand
     * @param b Second operand
     * @return Sum of a and b
     * 
     * @code
     * Calculator calc;
     * double result = calc.add(10.5, 5.3);
     * @endcode
     */
    double add(double a, double b) const;

    /**
     * @brief Subtract two numbers
     * @param a First operand (minuend)
     * @param b Second operand (subtrahend)
     * @return Difference (a - b)
     */
    double subtract(double a, double b) const;

    /**
     * @brief Multiply two numbers
     * @param a First operand
     * @param b Second operand
     * @return Product of a and b
     */
    double multiply(double a, double b) const;

    /**
     * @brief Divide two numbers
     * @param a Dividend
     * @param b Divisor
     * @return Quotient (a / b)
     * @throw std::invalid_argument if b is zero
     * 
     * @warning Division by zero will throw an exception
     */
    double divide(double a, double b) const;

    /**
     * @brief Calculate power of a number
     * @param base Base number
     * @param exponent Exponent value
     * @return base raised to the power of exponent
     */
    double power(double base, double exponent) const;

    /**
     * @brief Calculate square root
     * @param value Input value
     * @return Square root of value
     * @throw std::invalid_argument if value is negative
     */
    double squareRoot(double value) const;

    /**
     * @brief Calculate factorial of a number
     * @param n Non-negative integer
     * @return Factorial of n (n!)
     * @throw std::invalid_argument if n is negative
     * 
     * @note For large values of n, consider using a library that handles big integers
     */
    unsigned long long factorial(int n) const;

private:
    /// @brief Internal precision setting for calculations
    int precision_;

    /**
     * @brief Validate input for mathematical operations
     * @param value Value to validate
     * @return true if valid, false otherwise
     */
    bool validateInput(double value) const;
};

/**
 * @class Statistics
 * @brief Statistical operations on datasets
 * 
 * Provides methods for calculating various statistical measures
 * such as mean, median, mode, standard deviation, and variance.
 */
class Statistics {
public:
    /**
     * @brief Calculate the mean (average) of a dataset
     * @param data Vector of numbers
     * @return Mean value
     * @throw std::invalid_argument if data is empty
     * 
     * The mean is calculated as: \f$ \bar{x} = \frac{1}{n}\sum_{i=1}^{n}x_i \f$
     */
    static double mean(const std::vector<double>& data);

    /**
     * @brief Calculate the median of a dataset
     * @param data Vector of numbers
     * @return Median value
     * @throw std::invalid_argument if data is empty
     * 
     * The median is the middle value when data is sorted.
     * For even-length datasets, it's the average of the two middle values.
     */
    static double median(std::vector<double> data);

    /**
     * @brief Calculate the standard deviation
     * @param data Vector of numbers
     * @param sample If true, calculates sample standard deviation (n-1), otherwise population (n)
     * @return Standard deviation
     * @throw std::invalid_argument if data is empty or has only one element (for sample std dev)
     * 
     * Standard deviation formula: \f$ \sigma = \sqrt{\frac{\sum(x_i - \bar{x})^2}{n}} \f$
     */
    static double standardDeviation(const std::vector<double>& data, bool sample = true);

    /**
     * @brief Calculate the variance
     * @param data Vector of numbers
     * @param sample If true, calculates sample variance, otherwise population variance
     * @return Variance value
     * 
     * Variance is the square of standard deviation.
     */
    static double variance(const std::vector<double>& data, bool sample = true);

    /**
     * @brief Find the minimum value in a dataset
     * @param data Vector of numbers
     * @return Minimum value
     * @throw std::invalid_argument if data is empty
     */
    static double min(const std::vector<double>& data);

    /**
     * @brief Find the maximum value in a dataset
     * @param data Vector of numbers
     * @return Maximum value
     * @throw std::invalid_argument if data is empty
     */
    static double max(const std::vector<double>& data);

    /**
     * @brief Calculate the range of a dataset
     * @param data Vector of numbers
     * @return Range (max - min)
     * @throw std::invalid_argument if data is empty
     */
    static double range(const std::vector<double>& data);
};

/**
 * @enum MathError
 * @brief Error codes for mathematical operations
 */
enum class MathError {
    NONE = 0,           ///< No error
    DIVISION_BY_ZERO,   ///< Division by zero attempted
    INVALID_INPUT,      ///< Invalid input provided
    OVERFLOW,           ///< Numerical overflow
    UNDERFLOW,          ///< Numerical underflow
    DOMAIN_ERROR        ///< Domain error (e.g., sqrt of negative)
};

/**
 * @brief Convert MathError to string representation
 * @param error Error code
 * @return String description of the error
 */
const char* mathErrorToString(MathError error);

} // namespace MathOps

#endif // MATH_OPERATIONS_HPP
