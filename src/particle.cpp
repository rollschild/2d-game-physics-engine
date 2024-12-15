#include "particle.h"
#include "vec2.h"
#include <iostream>

Particle::Particle(float x, float y, float mass) {
    this->position = Vec2(x, y);
    this->mass = mass;
    this->inv_mass = mass != 0.0 ? (1.0 / mass) : 0.0;
    std::cout << "Particle constructor called!" << std::endl;
}

Particle::~Particle() {
    std::cout << "Particle destructor called!" << std::endl;
}

void Particle::apply_force(const Vec2 &force) { sum_forces += force; }

void Particle::clear_forces() { sum_forces = Vec2(0.0, 0.0); }

void Particle::integrate(float dt) {
    acceleartion = sum_forces * inv_mass;

    velocity += acceleartion * dt;
    position += velocity * dt;

    clear_forces();
}
