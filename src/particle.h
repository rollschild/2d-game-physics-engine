#ifndef PARTICLE_H
#define PARTICLE_H

#include "vec2.h"

struct Particle {
    int radius;

    Vec2 position;
    Vec2 velocity;
    Vec2 acceleartion;

    float mass;

    Particle(float x, float y, float mass);
    ~Particle();

    // copy constructor
    Particle(const Particle &other) = default;

    void integrate(float dt);
};

#endif
