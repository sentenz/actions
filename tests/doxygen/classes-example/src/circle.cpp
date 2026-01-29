/**
 * @file circle.cpp
 * @brief Implementation of Circle class
 */

#include "circle.hpp"
#include <cmath>
#include <stdexcept>

namespace Geometry {

Circle::Circle(const Point& center, double radius)
    : center_(center), radius_(radius) {
    if (radius <= 0.0) {
        throw std::invalid_argument("Circle radius must be positive");
    }
}

double Circle::getArea() const {
    return M_PI * radius_ * radius_;
}

double Circle::getCircumference() const {
    return 2.0 * M_PI * radius_;
}

bool Circle::contains(const Point& point) const {
    double dx = point.x - center_.x;
    double dy = point.y - center_.y;
    double distance = std::sqrt(dx * dx + dy * dy);
    return distance <= radius_;
}

} // namespace Geometry
