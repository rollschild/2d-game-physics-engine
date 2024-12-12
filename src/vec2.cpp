#include "vec2.h"
#include <cmath>
#include <math.h>

Vec2::Vec2() : x(0.0), y(0.0) {}
Vec2::Vec2(float x, float y) : x(x), y(y) {}

void Vec2::add(const Vec2 &v) {
    x += v.x;
    y += v.y;
}

void Vec2::sub(const Vec2 &v) {
    x -= v.x;
    y -= v.y;
}

void Vec2::scale(const float n) {
    x *= n;
    y *= n;
}

Vec2 Vec2::rotate(const float angle) const {
    Vec2 result;
    result.x = x * cos(angle) - y * sin(angle);
    result.y = x * sin(angle) + y * cos(angle);
    return result;
}

float Vec2::mag() const { return sqrtf(x * x + y * y); }

float Vec2::mag_sqaure() const { return (x * x + y * y); }

Vec2 &Vec2::normalize() {
    float len = mag();
    if (len != 0.0) {
        x /= len;
        y /= len;
    }

    return *this;
}

Vec2 Vec2::unit_vector() const {
    Vec2 result = Vec2(0, 0);
    float len = mag();
    if (len != 0.0) {
        result.x = x / len;
        result.y = y / len;
    }
    return result;
}

Vec2 Vec2::normal() const { return Vec2(y, -x).normalize(); }

float Vec2::dot(const Vec2 &v) const { return (x * v.x) + (y * v.y); }

/**
 * Only return the z part
 */
float Vec2::cross(const Vec2 &v) const { return (x * v.y) - (y * v.x); }

Vec2 &Vec2::operator=(const Vec2 &v) {
    x = v.x;
    y = v.y;
    return *this;
}

bool Vec2::operator==(const Vec2 &v) const { return x == v.x && y == v.y; }

bool Vec2::operator!=(const Vec2 &v) const { return !(*this == v); }

Vec2 Vec2::operator+(const Vec2 &v) const {
    Vec2 result;
    result.x = x + v.x;
    result.y = y + v.y;
    return result;
}

Vec2 Vec2::operator-(const Vec2 &v) const {
    Vec2 result;
    result.x = x - v.x;
    result.y = y - v.y;
    return result;
}

Vec2 Vec2::operator*(const float n) const {
    Vec2 result;
    result.x = x * n;
    result.y = y * n;
    return result;
}

Vec2 Vec2::operator/(const float n) const {
    Vec2 result;
    result.x = x / n;
    result.y = y / n;
    return result;
}

Vec2 &Vec2::operator-=(const Vec2 &v) {
    x -= v.x;
    y -= v.y;
    return *this;
}

Vec2 &Vec2::operator+=(const Vec2 &v) {
    x += v.x;
    y += v.y;
    return *this;
}

Vec2 &Vec2::operator*=(const float n) {
    x *= n;
    y *= n;
    return *this;
}

Vec2 &Vec2::operator/=(const float n) {
    x /= n;
    y /= n;
    return *this;
}

Vec2 Vec2::operator-() {
    Vec2 result;
    result.x = x * -1;
    result.y = y * -1;
    return result;
}
