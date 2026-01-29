/**
 * @file shape.cpp
 * @brief Implementation of shape classes
 */

#include "shape.hpp"
#include <cmath>
#include <stdexcept>

// Rectangle implementation
Rectangle::Rectangle(double width, double height)
    : width_(width), height_(height) {
    if (width <= 0.0 || height <= 0.0) {
        throw std::invalid_argument("Rectangle dimensions must be positive");
    }
}

double Rectangle::area() const {
    return width_ * height_;
}

double Rectangle::perimeter() const {
    return 2.0 * (width_ + height_);
}

std::string Rectangle::getName() const {
    return "Rectangle";
}

// Triangle implementation
Triangle::Triangle(double a, double b, double c)
    : a_(a), b_(b), c_(c) {
    // Check triangle inequality theorem
    if (a <= 0.0 || b <= 0.0 || c <= 0.0) {
        throw std::invalid_argument("Triangle sides must be positive");
    }
    if (a + b <= c || b + c <= a || a + c <= b) {
        throw std::invalid_argument("Sides do not form a valid triangle");
    }
}

double Triangle::area() const {
    // Heron's formula
    double s = (a_ + b_ + c_) / 2.0;
    return std::sqrt(s * (s - a_) * (s - b_) * (s - c_));
}

double Triangle::perimeter() const {
    return a_ + b_ + c_;
}

std::string Triangle::getName() const {
    return "Triangle";
}
