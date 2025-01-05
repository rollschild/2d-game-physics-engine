#include "body.h"
#include "shape.h"
#include "vec2.h"
#include <iostream>

// Remember to initialize the member variables in the _same_ order that they are
// declared. Otherwise you will get those `-Werror=reorder` compile errors
Body::Body(const Shape &shape, float x, float y, float mass)
    : position(Vec2(x, y)), velocity(Vec2(0, 0)), acceleartion(Vec2(0, 0)),
      rotation(0.0), angular_vel(0.0), angular_acc(0.0), sum_forces(Vec2(0, 0)),
      sum_torque(0.0), mass(mass), shape(shape.clone()) {
    inv_mass = mass != 0.0 ? (1.0 / mass) : 0.0;

    I = this->shape->get_moment_of_inertia() * mass;
    inv_I = I != 0.0 ? (1.0 / I) : 0.0;

    std::cout << "Body constructor called!" << std::endl;
}

Body::~Body() {
    delete shape;
    std::cout << "Body destructor called!" << std::endl;
}

void Body::apply_force(const Vec2 &force) { sum_forces += force; }

void Body::apply_torque(const float torque) { sum_torque += torque; }

void Body::clear_forces() { sum_forces = Vec2(0.0, 0.0); }

void Body::clear_torque() { sum_torque = 0.0; }

void Body::integrate_linear(float dt) {
    acceleartion = sum_forces * inv_mass;

    velocity += acceleartion * dt;
    position += velocity * dt;

    clear_forces();
}

void Body::integrate_angular(float dt) {
    angular_acc = sum_torque * inv_I;

    angular_vel += angular_acc * dt;
    rotation += angular_vel * dt;

    clear_torque();
}

void Body::update(float dt) {
    integrate_linear(dt);
    integrate_angular(dt);

    bool is_polygon = shape->get_type() == POLYGON || shape->get_type() == BOX;
    if (is_polygon) {
        PolygonShape *polygon_shape = (PolygonShape *)shape;
        polygon_shape->update_vertices(rotation, position);
    }
}
