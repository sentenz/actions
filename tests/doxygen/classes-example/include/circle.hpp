/**
 * @file circle.hpp
 * @brief Circle class definition
 * @author Example Team
 * @version 1.0.0
 */

#ifndef CIRCLE_HPP
#define CIRCLE_HPP

/**
 * @namespace Geometry
 * @brief Geometric shapes and operations
 */
namespace Geometry {

/**
 * @struct Point
 * @brief Represents a point in 2D space
 */
struct Point {
    double x; ///< X coordinate
    double y; ///< Y coordinate

    /**
     * @brief Default constructor - initializes to origin
     */
    Point() : x(0.0), y(0.0) {}

    /**
     * @brief Parameterized constructor
     * @param x_val X coordinate
     * @param y_val Y coordinate
     */
    Point(double x_val, double y_val) : x(x_val), y(y_val) {}
};

/**
 * @class Circle
 * @brief Represents a circle in 2D space
 * 
 * A circle is defined by a center point and a radius.
 * This class provides methods to calculate area and circumference.
 * 
 * Example usage:
 * @code
 * Circle c(Point(0, 0), 5.0);
 * double area = c.getArea();
 * double circumference = c.getCircumference();
 * @endcode
 */
class Circle {
public:
    /**
     * @brief Construct a circle
     * @param center Center point of the circle
     * @param radius Radius of the circle
     * @throw std::invalid_argument if radius is negative or zero
     */
    Circle(const Point& center, double radius);

    /**
     * @brief Get the center point
     * @return Center point of the circle
     */
    Point getCenter() const { return center_; }

    /**
     * @brief Get the radius
     * @return Radius value
     */
    double getRadius() const { return radius_; }

    /**
     * @brief Calculate area of the circle
     * @return Area (π × r²)
     * 
     * The area is calculated using the formula: A = π × r²
     */
    double getArea() const;

    /**
     * @brief Calculate circumference of the circle
     * @return Circumference (2 × π × r)
     * 
     * The circumference is calculated using: C = 2 × π × r
     */
    double getCircumference() const;

    /**
     * @brief Check if a point is inside the circle
     * @param point Point to check
     * @return true if point is inside or on the circle, false otherwise
     */
    bool contains(const Point& point) const;

private:
    Point center_;  ///< Center point of the circle
    double radius_; ///< Radius of the circle
};

} // namespace Geometry

#endif // CIRCLE_HPP
