#include "shape.h"
#include "vec2.h"
#include <iostream>
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
