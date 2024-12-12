#ifndef VEC2_H
#define VEC2_H

struct Vec2 {
    float x;
    float y;

    Vec2();
    Vec2(float x, float y);

    // we need to explicitly define (default) copy constructor because
    // we have a custom copy assignment operator `operator=`
    Vec2(const Vec2 &other) = default;

    ~Vec2() = default;

    void add(const Vec2 &v);
    void sub(const Vec2 &v);
    void scale(const float n);
    Vec2 rotate(const float angle) const;

    float mag() const;
    float mag_sqaure() const;

    Vec2 &normalize();
    Vec2 unit_vector() const;
    Vec2 normal() const;

    float dot(const Vec2 &v) const;
    float cross(const Vec2 &v) const;

    // operator overloads
    Vec2 &operator=(const Vec2 &v); // v1 = v2
    bool operator==(const Vec2 &v) const;
    bool operator!=(const Vec2 &v) const;

    Vec2 operator+(const Vec2 &v) const;
    Vec2 operator-(const Vec2 &v) const;
    Vec2 operator*(const float n) const;
    Vec2 operator/(const float n) const;
    Vec2 operator-();

    Vec2 &operator+=(const Vec2 &v);
    Vec2 &operator-=(const Vec2 &v);
    Vec2 &operator*=(const float n);
    Vec2 &operator/=(const float n);
};

#endif
