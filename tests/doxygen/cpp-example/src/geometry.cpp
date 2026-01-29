/**
 * @file geometry.cpp
 * @brief Implementation of geometric shapes
 */

#include "geometry.hpp"
#include <sstream>
#include <cmath>
#include <stdexcept>

namespace Geometry {

// Point2D implementation
double Point2D::distanceTo(const Point2D& other) const {
    double dx = x - other.x;
    double dy = y - other.y;
    return std::sqrt(dx * dx + dy * dy);
}

// Circle implementation
Circle::Circle(const Point2D& center, double radius)
    : center_(center), radius_(radius) {
    if (radius <= 0.0) {
        throw std::invalid_argument("Circle radius must be positive");
    }
}

double Circle::area() const {
    return M_PI * radius_ * radius_;
}

double Circle::perimeter() const {
    return 2.0 * M_PI * radius_;
}

std::string Circle::getName() const {
    return "Circle";
}

std::string Circle::describe() const {
    std::ostringstream oss;
    oss << "Circle(center: (" << center_.x << ", " << center_.y
        << "), radius: " << radius_ << ")";
    return oss.str();
}

bool Circle::contains(const Point2D& point) const {
    return center_.distanceTo(point) <= radius_;
}

// Rectangle implementation
Rectangle::Rectangle(const Point2D& topLeft, double width, double height)
    : topLeft_(topLeft), width_(width), height_(height) {
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

std::string Rectangle::describe() const {
    std::ostringstream oss;
    oss << "Rectangle(topLeft: (" << topLeft_.x << ", " << topLeft_.y
        << "), width: " << width_ << ", height: " << height_ << ")";
    return oss.str();
}

bool Rectangle::contains(const Point2D& point) const {
    return point.x >= topLeft_.x && point.x <= topLeft_.x + width_ &&
           point.y >= topLeft_.y && point.y <= topLeft_.y + height_;
}

bool Rectangle::intersects(const Rectangle& other) const {
    double x1 = topLeft_.x;
    double y1 = topLeft_.y;
    double x2 = x1 + width_;
    double y2 = y1 + height_;

    double ox1 = other.topLeft_.x;
    double oy1 = other.topLeft_.y;
    double ox2 = ox1 + other.width_;
    double oy2 = oy1 + other.height_;

    return !(x2 < ox1 || x1 > ox2 || y2 < oy1 || y1 > oy2);
}

// Triangle implementation
Triangle::Triangle(const Point2D& a, const Point2D& b, const Point2D& c)
    : a_(a), b_(b), c_(c) {
    // Check if points are collinear
    double area = std::abs((b.x - a.x) * (c.y - a.y) - (c.x - a.x) * (b.y - a.y));
    if (area < 1e-10) {
        throw std::invalid_argument("Triangle vertices cannot be collinear");
    }
}

double Triangle::sideAB() const {
    return a_.distanceTo(b_);
}

double Triangle::sideBC() const {
    return b_.distanceTo(c_);
}

double Triangle::sideCA() const {
    return c_.distanceTo(a_);
}

double Triangle::area() const {
    // Using Heron's formula
    double a = sideAB();
    double b = sideBC();
    double c = sideCA();
    double s = (a + b + c) / 2.0;
    return std::sqrt(s * (s - a) * (s - b) * (s - c));
}

double Triangle::perimeter() const {
    return sideAB() + sideBC() + sideCA();
}

std::string Triangle::getName() const {
    return "Triangle";
}

std::string Triangle::describe() const {
    std::ostringstream oss;
    oss << "Triangle(A: (" << a_.x << ", " << a_.y << "), "
        << "B: (" << b_.x << ", " << b_.y << "), "
        << "C: (" << c_.x << ", " << c_.y << "))";
    return oss.str();
}

bool Triangle::isEquilateral() const {
    double a = sideAB();
    double b = sideBC();
    double c = sideCA();
    const double epsilon = 1e-10;
    return std::abs(a - b) < epsilon && std::abs(b - c) < epsilon;
}

bool Triangle::isIsosceles() const {
    double a = sideAB();
    double b = sideBC();
    double c = sideCA();
    const double epsilon = 1e-10;
    return std::abs(a - b) < epsilon || std::abs(b - c) < epsilon || std::abs(a - c) < epsilon;
}

bool Triangle::isRight() const {
    double a = sideAB();
    double b = sideBC();
    double c = sideCA();
    const double epsilon = 1e-10;

    // Check Pythagorean theorem for all combinations
    return std::abs(a*a + b*b - c*c) < epsilon ||
           std::abs(b*b + c*c - a*a) < epsilon ||
           std::abs(a*a + c*c - b*b) < epsilon;
}

double polygonArea(const std::vector<Point2D>& vertices) {
    if (vertices.size() < 3) {
        throw std::invalid_argument("Polygon must have at least 3 vertices");
    }

    // Shoelace formula
    double area = 0.0;
    size_t n = vertices.size();
    for (size_t i = 0; i < n; ++i) {
        size_t j = (i + 1) % n;
        area += vertices[i].x * vertices[j].y;
        area -= vertices[j].x * vertices[i].y;
    }
    return std::abs(area) / 2.0;
}

} // namespace Geometry
