/**
 * @file geometry.hpp
 * @brief Geometric shapes and calculations
 * @author Example Team
 * @version 1.0.0
 * @date 2024-01-29
 * 
 * This file provides classes for working with 2D geometric shapes
 * including points, circles, rectangles, and triangles.
 */

#ifndef GEOMETRY_HPP
#define GEOMETRY_HPP

#include <cmath>
#include <string>
#include <vector>

/**
 * @namespace Geometry
 * @brief Namespace for geometric operations
 */
namespace Geometry {

/**
 * @struct Point2D
 * @brief Represents a point in 2D space
 * 
 * A simple structure to hold x and y coordinates.
 */
struct Point2D {
    double x; ///< X coordinate
    double y; ///< Y coordinate

    /**
     * @brief Default constructor - initializes to origin
     */
    Point2D() : x(0.0), y(0.0) {}

    /**
     * @brief Parameterized constructor
     * @param x_val X coordinate
     * @param y_val Y coordinate
     */
    Point2D(double x_val, double y_val) : x(x_val), y(y_val) {}

    /**
     * @brief Calculate distance to another point
     * @param other The other point
     * @return Euclidean distance
     * 
     * Uses the formula: \f$ d = \sqrt{(x_2-x_1)^2 + (y_2-y_1)^2} \f$
     */
    double distanceTo(const Point2D& other) const;
};

/**
 * @class Shape
 * @brief Abstract base class for all shapes
 * 
 * This is an abstract class that defines the interface
 * for all geometric shapes.
 */
class Shape {
public:
    /**
     * @brief Virtual destructor
     */
    virtual ~Shape() {}

    /**
     * @brief Calculate the area of the shape
     * @return Area in square units
     */
    virtual double area() const = 0;

    /**
     * @brief Calculate the perimeter of the shape
     * @return Perimeter in linear units
     */
    virtual double perimeter() const = 0;

    /**
     * @brief Get the name of the shape
     * @return Shape name as string
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Get a description of the shape
     * @return Detailed description string
     */
    virtual std::string describe() const = 0;
};

/**
 * @class Circle
 * @brief Represents a circle in 2D space
 * 
 * A circle is defined by a center point and a radius.
 * 
 * @see Shape
 */
class Circle : public Shape {
public:
    /**
     * @brief Construct a circle
     * @param center Center point of the circle
     * @param radius Radius of the circle
     * @throw std::invalid_argument if radius is negative or zero
     */
    Circle(const Point2D& center, double radius);

    /**
     * @brief Get the center point
     * @return Center point
     */
    Point2D getCenter() const { return center_; }

    /**
     * @brief Get the radius
     * @return Radius value
     */
    double getRadius() const { return radius_; }

    /**
     * @brief Calculate area of the circle
     * @return Area (π * r²)
     */
    double area() const override;

    /**
     * @brief Calculate circumference of the circle
     * @return Circumference (2 * π * r)
     */
    double perimeter() const override;

    /**
     * @brief Get the name of the shape
     * @return "Circle"
     */
    std::string getName() const override;

    /**
     * @brief Get a description of the circle
     * @return Description including center and radius
     */
    std::string describe() const override;

    /**
     * @brief Check if a point is inside the circle
     * @param point Point to check
     * @return true if point is inside or on the circle, false otherwise
     */
    bool contains(const Point2D& point) const;

private:
    Point2D center_; ///< Center point of the circle
    double radius_;  ///< Radius of the circle
};

/**
 * @class Rectangle
 * @brief Represents a rectangle in 2D space
 * 
 * A rectangle is defined by its top-left corner, width, and height.
 */
class Rectangle : public Shape {
public:
    /**
     * @brief Construct a rectangle
     * @param topLeft Top-left corner point
     * @param width Width of the rectangle
     * @param height Height of the rectangle
     * @throw std::invalid_argument if width or height is negative or zero
     */
    Rectangle(const Point2D& topLeft, double width, double height);

    /**
     * @brief Get the top-left corner
     * @return Top-left point
     */
    Point2D getTopLeft() const { return topLeft_; }

    /**
     * @brief Get the width
     * @return Width value
     */
    double getWidth() const { return width_; }

    /**
     * @brief Get the height
     * @return Height value
     */
    double getHeight() const { return height_; }

    /**
     * @brief Calculate area of the rectangle
     * @return Area (width * height)
     */
    double area() const override;

    /**
     * @brief Calculate perimeter of the rectangle
     * @return Perimeter (2 * (width + height))
     */
    double perimeter() const override;

    /**
     * @brief Get the name of the shape
     * @return "Rectangle"
     */
    std::string getName() const override;

    /**
     * @brief Get a description of the rectangle
     * @return Description including position and dimensions
     */
    std::string describe() const override;

    /**
     * @brief Check if a point is inside the rectangle
     * @param point Point to check
     * @return true if point is inside or on the rectangle, false otherwise
     */
    bool contains(const Point2D& point) const;

    /**
     * @brief Check if this rectangle intersects with another
     * @param other The other rectangle
     * @return true if rectangles intersect, false otherwise
     */
    bool intersects(const Rectangle& other) const;

private:
    Point2D topLeft_; ///< Top-left corner
    double width_;    ///< Width of the rectangle
    double height_;   ///< Height of the rectangle
};

/**
 * @class Triangle
 * @brief Represents a triangle in 2D space
 * 
 * A triangle is defined by three vertices.
 */
class Triangle : public Shape {
public:
    /**
     * @brief Construct a triangle from three points
     * @param a First vertex
     * @param b Second vertex
     * @param c Third vertex
     * @throw std::invalid_argument if points are collinear
     */
    Triangle(const Point2D& a, const Point2D& b, const Point2D& c);

    /**
     * @brief Get vertex A
     * @return First vertex
     */
    Point2D getVertexA() const { return a_; }

    /**
     * @brief Get vertex B
     * @return Second vertex
     */
    Point2D getVertexB() const { return b_; }

    /**
     * @brief Get vertex C
     * @return Third vertex
     */
    Point2D getVertexC() const { return c_; }

    /**
     * @brief Calculate area of the triangle using Heron's formula
     * @return Area in square units
     * 
     * Area is calculated using: \f$ A = \sqrt{s(s-a)(s-b)(s-c)} \f$
     * where s is the semi-perimeter.
     */
    double area() const override;

    /**
     * @brief Calculate perimeter of the triangle
     * @return Sum of all three sides
     */
    double perimeter() const override;

    /**
     * @brief Get the name of the shape
     * @return "Triangle"
     */
    std::string getName() const override;

    /**
     * @brief Get a description of the triangle
     * @return Description including vertices
     */
    std::string describe() const override;

    /**
     * @brief Check if the triangle is equilateral
     * @return true if all sides are equal
     */
    bool isEquilateral() const;

    /**
     * @brief Check if the triangle is isosceles
     * @return true if at least two sides are equal
     */
    bool isIsosceles() const;

    /**
     * @brief Check if the triangle is right-angled
     * @return true if triangle has a 90-degree angle
     */
    bool isRight() const;

private:
    Point2D a_; ///< First vertex
    Point2D b_; ///< Second vertex
    Point2D c_; ///< Third vertex

    /**
     * @brief Calculate length of side AB
     * @return Length of side AB
     */
    double sideAB() const;

    /**
     * @brief Calculate length of side BC
     * @return Length of side BC
     */
    double sideBC() const;

    /**
     * @brief Calculate length of side CA
     * @return Length of side CA
     */
    double sideCA() const;
};

/**
 * @brief Calculate the area of a polygon given its vertices
 * @param vertices Vector of points representing the polygon vertices
 * @return Area using the shoelace formula
 * @throw std::invalid_argument if fewer than 3 vertices
 * 
 * Uses the shoelace formula for calculating polygon area.
 */
double polygonArea(const std::vector<Point2D>& vertices);

} // namespace Geometry

#endif // GEOMETRY_HPP
