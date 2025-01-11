#include "shape.h"
#include "vec2.h"
#include <iostream>
#include <limits>
#include <vector>

CircleShape::CircleShape(float radius) : radius(radius) {
    std::cout << "CircleShape constructor called!" << std::endl;
}

CircleShape::~CircleShape() {
    std::cout << "CircleShape destructor called!" << std::endl;
}

float CircleShape::get_moment_of_inertia() const {
    // remember, to be multiplied by `mass`
    return 0.5 * (radius * radius);
}

ShapeType CircleShape::get_type() const { return CIRCLE; }

Shape *CircleShape::clone() const { return new CircleShape(radius); }

PolygonShape::PolygonShape(const std::vector<Vec2> vertices)
    : local_vertices(vertices), world_vertices(vertices) {
    std::cout << "PolygonShape constructor called!" << std::endl;
}

PolygonShape::~PolygonShape() {
    std::cout << "PolygonShape destructor called!" << std::endl;
}

ShapeType PolygonShape::get_type() const { return POLYGON; }

Shape *PolygonShape::clone() const { return new PolygonShape(local_vertices); }

float PolygonShape::get_moment_of_inertia() const {
    // TODO
    return 0.0;
}

void PolygonShape::update_vertices(float angle, const Vec2 &position) {
    for (size_t i = 0; i < local_vertices.size(); i++) {
        // first, rotate
        world_vertices[i] = local_vertices[i].rotate(angle);

        // then, translate
        world_vertices[i] += position;
    }
}

Vec2 PolygonShape::edge_at(const int index) const {
    int curr_vertex = index;
    int next_vertex = (index + 1) % world_vertices.size();

    return world_vertices[next_vertex] - world_vertices[curr_vertex];
}

float PolygonShape::find_min_separation(const PolygonShape *other, Vec2 &axis,
                                        Vec2 &point) const {
    // Loop all vertices of a
    //   - find normal axis (perpendicular)
    //   - loop all vertices of b
    //     - project vertex b onto the normal axis
    //     - keep track of the min separation
    // Return the best separation out of all axis
    float separation = std::numeric_limits<float>::lowest();
    // separation is also negative collision depth

    for (size_t i = 0; i < this->world_vertices.size(); i++) {
        Vec2 va = this->world_vertices[i];
        Vec2 normal = this->edge_at(i).normal();
        float min_sep = std::numeric_limits<float>::max();
        Vec2 min_vertex;

        for (size_t j = 0; j < other->world_vertices.size(); j++) {
            Vec2 vb = other->world_vertices[j];
            float proj = (vb - va).dot(normal);
            if (proj < min_sep) {
                min_sep = proj;
                min_vertex = vb;
            }
            min_sep = std::min(min_sep, (vb - va).dot(normal));
        }

        if (separation < min_sep) {
            separation = min_sep;
            axis = this->edge_at(i);
            point = min_vertex;
        }
    }

    return separation;
}

BoxShape::BoxShape(float width, float height) : width(width), height(height) {
    // load vertices of the box polygon
    local_vertices.push_back(Vec2(-width / 2.0, -height / 2.0));
    local_vertices.push_back(Vec2(width / 2.0, -height / 2.0));
    local_vertices.push_back(Vec2(width / 2.0, height / 2.0));
    local_vertices.push_back(Vec2(-width / 2.0, height / 2.0));

    world_vertices.push_back(Vec2(-width / 2.0, -height / 2.0));
    world_vertices.push_back(Vec2(width / 2.0, -height / 2.0));
    world_vertices.push_back(Vec2(width / 2.0, height / 2.0));
    world_vertices.push_back(Vec2(-width / 2.0, height / 2.0));
    std::cout << "BoxShape constructor called!" << std::endl;
}

BoxShape::~BoxShape() {
    std::cout << "BoxShape destructor called!" << std::endl;
}

ShapeType BoxShape::get_type() const { return BOX; }

Shape *BoxShape::clone() const { return new BoxShape(width, height); }

float BoxShape::get_moment_of_inertia() const {
    // remember, to be multiplied by `mass`
    return (1.0 / 12.0) * (width * width + height * height);
}
