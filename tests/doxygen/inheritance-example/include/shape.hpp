/**
 * @file shape.hpp
 * @brief Abstract shape hierarchy
 * @author Example Team
 * @version 1.0.0
 */

#ifndef SHAPE_HPP
#define SHAPE_HPP

#include <string>

/**
 * @class Shape
 * @brief Abstract base class for all geometric shapes
 * 
 * This class defines the interface that all shapes must implement.
 * It demonstrates object-oriented programming with virtual functions
 * and abstract base classes.
 * 
 * @see Rectangle
 * @see Triangle
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
     * 
     * This is a pure virtual function that must be implemented
     * by derived classes.
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
};

/**
 * @class Rectangle
 * @brief Represents a rectangle shape
 * 
 * A rectangle is defined by its width and height.
 * 
 * @see Shape
 */
class Rectangle : public Shape {
public:
    /**
     * @brief Construct a rectangle
     * @param width Width of the rectangle
     * @param height Height of the rectangle
     * @throw std::invalid_argument if width or height is negative or zero
     */
    Rectangle(double width, double height);

    /**
     * @brief Calculate area of the rectangle
     * @return Area (width × height)
     */
    double area() const override;

    /**
     * @brief Calculate perimeter of the rectangle
     * @return Perimeter (2 × (width + height))
     */
    double perimeter() const override;

    /**
     * @brief Get the name of the shape
     * @return "Rectangle"
     */
    std::string getName() const override;

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

private:
    double width_;  ///< Width of the rectangle
    double height_; ///< Height of the rectangle
};

/**
 * @class Triangle
 * @brief Represents a triangle shape
 * 
 * A triangle is defined by the lengths of its three sides.
 * 
 * @see Shape
 */
class Triangle : public Shape {
public:
    /**
     * @brief Construct a triangle from three side lengths
     * @param a Length of first side
     * @param b Length of second side
     * @param c Length of third side
     * @throw std::invalid_argument if sides don't form a valid triangle
     * 
     * @note The triangle inequality theorem is checked: the sum of any
     * two sides must be greater than the third side.
     */
    Triangle(double a, double b, double c);

    /**
     * @brief Calculate area using Heron's formula
     * @return Area in square units
     * 
     * Area is calculated using Heron's formula:
     * A = √(s(s-a)(s-b)(s-c))
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

private:
    double a_; ///< Length of first side
    double b_; ///< Length of second side
    double c_; ///< Length of third side
};

#endif // SHAPE_HPP
