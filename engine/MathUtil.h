#ifndef MATHUTIL_H
#define MATHUTIL_H

#include <algorithm>
#include <iostream>

namespace ECSEngine {

#include <cstdio>

/**
 * A 2D point or vector.
 */
struct Point2D {
    float x, y;

    /**
     * Create a point.
     * @param x The x coordinate.
     * @param y The y coordinate.
     */
    Point2D(float x = 0, float y = 0) : x(x), y(y) {}

    /**
     * Create a point from integer coordinates.
     * @param x The x coordinate.
     * @param y The y coordinate.
     */
    Point2D(int x, int y) : x(x), y(y) {}

    /**
     * Add another point to this point.
     * @param other The point to add.
     * @return This point.
     */
    Point2D& operator+=(const Point2D& other) {
        x += other.x;
        y += other.y;
        return *this;
    }
};

/**
 * Write a point to a stream as (x, y).
 * @param os The output stream.
 * @param p The point to write.
 * @return The output stream.
 */
[[maybe_unused]] static std::ostream& operator<<(std::ostream& os, const Point2D& p) {
    os << "(" << p.x << ", " << p.y << ")";
    return os;
}

/**
 * Scale a point by a number.
 * @param number The scale factor.
 * @param rhs The point to scale.
 * @return The scaled point.
 */
[[maybe_unused]] static Point2D operator*(float number, const Point2D& rhs) {
    return Point2D(number * rhs.x, number * rhs.y);
}

/**
 * Add two points.
 * @param lhs The first point.
 * @param rhs The second point.
 * @return The sum of the points.
 */
[[maybe_unused]] static Point2D operator+(const Point2D& lhs, const Point2D& rhs) {
    return Point2D(lhs.x + rhs.x, lhs.y + rhs.y);
}

/**
 * Subtract one point from another.
 * @param lhs The point to subtract from.
 * @param rhs The point to subtract.
 * @return The difference of the points.
 */
[[maybe_unused]] static Point2D operator-(const Point2D& lhs, const Point2D& rhs) {
    return Point2D(lhs.x - rhs.x, lhs.y - rhs.y);
}

/**
 * A line segment between two points.
 */
struct Line {
    Point2D p1, p2;

    /**
     * Create a line from two points.
     * @param p1 The start point.
     * @param p2 The end point.
     */
    Line(Point2D p1 = {0, 0}, Point2D p2 = {0, 0}) : p1(p1), p2(p2) {}

    /**
     * Create a line from two pairs of coordinates.
     * @param x1 The x coordinate of the start point.
     * @param y1 The y coordinate of the start point.
     * @param x2 The x coordinate of the end point.
     * @param y2 The y coordinate of the end point.
     */
    Line(float x1, float y1, float x2, float y2) : p1(x1, y1), p2(x2, y2) {}
};

/**
 * Write a line to a stream as L{p1, p2}.
 * @param os The output stream.
 * @param l The line to write.
 * @return The output stream.
 */
[[maybe_unused]] static std::ostream& operator<<(std::ostream& os, const Line& l) {
    os << "L{" << l.p1 << ", " << l.p2 << "}";
    return os;
}

/**
 * A circle with a center and radius.
 */
struct Circle {
    Point2D center;
    float radius;

    /**
     * Create a circle.
     * @param c The center of the circle.
     * @param r The radius of the circle.
     */
    Circle(Point2D c = {0, 0}, float r = 0) : center(c), radius(r) {}

    /**
     * Create a circle from center coordinates.
     * @param x The x coordinate of the center.
     * @param y The y coordinate of the center.
     * @param r The radius of the circle.
     */
    Circle(float x, float y, float r) : center(x, y), radius(r) {}
};

/**
 * An axis-aligned rectangle. Y grows downward, so the bottom edge is topLeft.y + height.
 */
struct Rect {
    Point2D topLeft;
    float width, height;

    /**
     * Set this rectangle to its overlap with another rectangle.
     * If they do not overlap, the width and height are set to 0.
     * @param other The rectangle to intersect with.
     * @return This rectangle.
     */
    Rect& operator&=(const Rect& other) {
        float left = std::max(topLeft.x, other.topLeft.x);
        float top = std::max(topLeft.y, other.topLeft.y);
        float right = std::min(topLeft.x + width, other.topLeft.x + other.width);
        float bot = std::min(topLeft.y + height, other.topLeft.y + other.height);

        // kinda confusing because we work in bottom right of grid but top < bot actually means the
        // opposite of the wording bottom will be a larger number if the rect is allowed
        if (left > right || top > bot) {
            width = 0;
            height = 0;
            return *this;
        }
        topLeft = {left, top};
        width = right - left;
        height = bot - top;
        return *this;
    }

    /**
     * Check whether this rectangle overlaps another. Touching edges do not count.
     * @param other The rectangle to check against.
     * @return True if the overlap has a positive width and height.
     */
    bool Intersect(const Rect& other) const {
        Rect output = *this;
        output &= other;
        if (output.width > 0 && output.height > 0) {  // using && for player on floor edge case
            return true;
        }
        return false;
    }
};

/**
 * Write a rectangle to a stream as R{topLeft, widthxheight}.
 * @param os The output stream.
 * @param r The rectangle to write.
 * @return The output stream.
 */
[[maybe_unused]] static std::ostream& operator<<(std::ostream& os, const Rect& r) {
    os << "R{" << r.topLeft << ", " << r.width << "x" << r.height << "}";
    return os;
}

}  // namespace ECSEngine

#endif  // MATHUTIL_H
