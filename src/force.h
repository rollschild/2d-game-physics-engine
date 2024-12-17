#ifndef FORCE_H
#define FORCE_H

#include "particle.h"
#include "vec2.h"

struct Force {
    static Vec2 generate_drag_force(const Particle &particle, float k);

    static Vec2 generate_firction_force(const Particle &particle, float k);

    static Vec2 generate_g_force(const Particle &a, const Particle &b, float G,
                                 float min_distance, float max_distance);
};

#endif
