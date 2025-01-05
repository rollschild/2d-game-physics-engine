#ifndef SHAPE_H
#define SHAPE_H

#include "vec2.h"
#include <vector>

enum ShapeType { CIRCLE, POLYGON, BOX };

struct Shape {
    virtual ~Shape() = default;
    virtual ShapeType get_type() const = 0; // pure virtual method
    virtual Shape *clone() const = 0;
    // virtual float get_moment_of_inertia() const = 0;

    virtual float get_moment_of_inertia() const = 0;
};

struct CircleShape : public Shape {
    float radius;

    CircleShape(const float radius);
    virtual ~CircleShape();
    ShapeType get_type() const override;
    Shape *clone() const override;

    float get_moment_of_inertia() const override;
};

struct PolygonShape : public Shape {
    std::vector<Vec2> local_vertices;
    std::vector<Vec2> world_vertices;

    PolygonShape() = default;
    PolygonShape(const std::vector<Vec2> vertices);
    virtual ~PolygonShape();
    ShapeType get_type() const override;
    Shape *clone() const override;

    float get_moment_of_inertia() const override;

    // rotate/translate polygon vertices from local space to world space
    void update_vertices(float rotation, const Vec2 &position);
};

struct BoxShape : public PolygonShape {
    float width;
    float height;
    BoxShape(float x, float y);
    virtual ~BoxShape();
    ShapeType get_type() const override;
    Shape *clone() const override;

    float get_moment_of_inertia() const override;
};

#endif
