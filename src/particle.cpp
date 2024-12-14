#include "particle.h"
#include <iostream>

Particle::Particle(float x, float y, float mass) {
    this->position = Vec2(x, y);
    this->mass = mass;
    std::cout << "Particle constructor called!" << std::endl;
}

Particle::~Particle() {
    std::cout << "Particle destructor called!" << std::endl;
}

void Particle::integrate(float dt) {
    velocity += acceleartion * dt;
    position += velocity * dt;
}
