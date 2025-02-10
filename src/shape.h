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

    virtual void update_vertices(float angle, const Vec2 &position) = 0;
};

struct CircleShape : public Shape {
    float radius;

    CircleShape(const float radius);
    virtual ~CircleShape();
    ShapeType get_type() const override;
    Shape *clone() const override;

    float get_moment_of_inertia() const override;

    void update_vertices(float angle, const Vec2 &position) override;
};

struct PolygonShape : public Shape {
    float width;
    float height;

    std::vector<Vec2> local_vertices;
    std::vector<Vec2> world_vertices;

    PolygonShape() = default;
    PolygonShape(const std::vector<Vec2> vertices);
    virtual ~PolygonShape();
    ShapeType get_type() const override;
    Shape *clone() const override;

    float get_moment_of_inertia() const override;

    Vec2 edge_at(const int index) const;
    float find_min_separation(const PolygonShape *other, int &index_ref_edge,
                              Vec2 &support_point) const;
    int find_incident_edge(const Vec2 &normal) const;

    int clip_segment_to_line(const std::vector<Vec2> &contacts_in,
                             std::vector<Vec2> &contacts_out, const Vec2 &c0,
                             const Vec2 &c1) const;

    // rotate/translate polygon vertices from local space to world space
    void update_vertices(float angle, const Vec2 &position) override;
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
