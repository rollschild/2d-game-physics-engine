#include "body.h"
#include "shape.h"
#include "vec2.h"
#include <iostream>

Body::Body(const Shape &shape, float x, float y, float mass) {
    this->shape = shape.clone();
    this->position = Vec2(x, y);
    this->mass = mass;
    this->inv_mass = mass != 0.0 ? (1.0 / mass) : 0.0;
    std::cout << "Body constructor called!" << std::endl;
}

Body::~Body() {
    delete shape;
    std::cout << "Body destructor called!" << std::endl;
}

void Body::apply_force(const Vec2 &force) { sum_forces += force; }

void Body::clear_forces() { sum_forces = Vec2(0.0, 0.0); }

void Body::integrate(float dt) {
    acceleartion = sum_forces * inv_mass;

    velocity += acceleartion * dt;
    position += velocity * dt;

    clear_forces();
}
