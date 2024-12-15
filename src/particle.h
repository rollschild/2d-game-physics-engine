#ifndef PARTICLE_H
#define PARTICLE_H

#include "vec2.h"

struct Particle {
    int radius;

    Vec2 position;
    Vec2 velocity;
    Vec2 acceleartion;

    Vec2 sum_forces;

    float mass;
    float inv_mass;

    Particle(float x, float y, float mass);
    ~Particle();

    // copy constructor
    Particle(const Particle &other) = default;

    void apply_force(const Vec2 &force);
    void clear_forces();

    void integrate(float dt);
};

#endif
