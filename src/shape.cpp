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

ShapeType CircleShape::get_type() const { return CIRCLE; }

Shape *CircleShape::clone() const { return new CircleShape(radius); }

PolygonShape::PolygonShape(const std::vector<Vec2> vertices)
    : vertices(vertices) {
    std::cout << "PolygonShape constructor called!" << std::endl;
}

PolygonShape::~PolygonShape() {
    std::cout << "PolygonShape destructor called!" << std::endl;
}

ShapeType PolygonShape::get_type() const { return POLYGON; }

Shape *PolygonShape::clone() const { return new PolygonShape(vertices); }

BoxShape::BoxShape(float width, float height) : width(width), height(height) {
    std::cout << "BoxShape constructor called!" << std::endl;
}

BoxShape::~BoxShape() {
    std::cout << "BoxShape destructor called!" << std::endl;
}

ShapeType BoxShape::get_type() const { return BOX; }

Shape *BoxShape::clone() const { return new BoxShape(width, height); }
