/**
 * @file calculator.hpp
 * @brief Basic calculator operations
 * @author Example Team
 * @version 1.0.0
 * @date 2024-01-29
 */

#ifndef CALCULATOR_HPP
#define CALCULATOR_HPP

/**
 * @brief Add two numbers
 * @param a First number
 * @param b Second number
 * @return Sum of a and b
 * 
 * @code
 * double result = add(5.0, 3.0);
 * // result will be 8.0
 * @endcode
 */
double add(double a, double b);

/**
 * @brief Subtract two numbers
 * @param a Minuend
 * @param b Subtrahend
 * @return Difference (a - b)
 */
double subtract(double a, double b);

/**
 * @brief Multiply two numbers
 * @param a First factor
 * @param b Second factor
 * @return Product of a and b
 */
double multiply(double a, double b);

/**
 * @brief Divide two numbers
 * @param a Dividend
 * @param b Divisor
 * @return Quotient (a / b)
 * @throw std::invalid_argument if b is zero
 * 
 * @warning Division by zero will throw an exception
 */
double divide(double a, double b);

#endif // CALCULATOR_HPP
