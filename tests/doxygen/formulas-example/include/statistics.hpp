/**
 * @file statistics.hpp
 * @brief Statistical calculations with mathematical formulas
 * @author Example Team
 * @version 1.0.0
 */

#ifndef STATISTICS_HPP
#define STATISTICS_HPP

#include <vector>

/**
 * @namespace Stats
 * @brief Statistical functions
 */
namespace Stats {

/**
 * @brief Calculate the mean (average) of a dataset
 * @param data Vector of numbers
 * @return Mean value
 * @throw std::invalid_argument if data is empty
 * 
 * The mean is calculated using the formula:
 * \f[
 *   \bar{x} = \frac{1}{n}\sum_{i=1}^{n}x_i
 * \f]
 * 
 * where \f$ n \f$ is the number of elements and \f$ x_i \f$ are the data points.
 */
double mean(const std::vector<double>& data);

/**
 * @brief Calculate the variance of a dataset
 * @param data Vector of numbers
 * @param sample If true, calculates sample variance (n-1), otherwise population variance (n)
 * @return Variance value
 * @throw std::invalid_argument if data is empty
 * 
 * Population variance formula:
 * \f[
 *   \sigma^2 = \frac{1}{n}\sum_{i=1}^{n}(x_i - \bar{x})^2
 * \f]
 * 
 * Sample variance formula:
 * \f[
 *   s^2 = \frac{1}{n-1}\sum_{i=1}^{n}(x_i - \bar{x})^2
 * \f]
 */
double variance(const std::vector<double>& data, bool sample = true);

/**
 * @brief Calculate the standard deviation
 * @param data Vector of numbers
 * @param sample If true, calculates sample standard deviation
 * @return Standard deviation
 * @throw std::invalid_argument if data is empty
 * 
 * Standard deviation is the square root of variance:
 * \f[
 *   \sigma = \sqrt{\sigma^2} = \sqrt{\frac{1}{n}\sum_{i=1}^{n}(x_i - \bar{x})^2}
 * \f]
 */
double standardDeviation(const std::vector<double>& data, bool sample = true);

/**
 * @brief Calculate the quadratic formula roots
 * @param a Coefficient of x²
 * @param b Coefficient of x
 * @param c Constant term
 * @return Vector containing the roots (may be 0, 1, or 2 roots)
 * 
 * Solves the quadratic equation:
 * \f[
 *   ax^2 + bx + c = 0
 * \f]
 * 
 * Using the quadratic formula:
 * \f[
 *   x = \frac{-b \pm \sqrt{b^2 - 4ac}}{2a}
 * \f]
 * 
 * The discriminant \f$ \Delta = b^2 - 4ac \f$ determines the number of real roots:
 * - If \f$ \Delta > 0 \f$: two distinct real roots
 * - If \f$ \Delta = 0 \f$: one repeated real root
 * - If \f$ \Delta < 0 \f$: no real roots (returns empty vector)
 */
std::vector<double> quadraticRoots(double a, double b, double c);

} // namespace Stats

#endif // STATISTICS_HPP
