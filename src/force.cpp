#include "force.h"
#include "particle.h"
#include "vec2.h"
#include <algorithm>

Vec2 Force::generate_drag_force(const Particle &particle, float k) {
    Vec2 drage_force = Vec2(0.0, 0.0);
    if (particle.velocity.mag_sqaure() > 0) {
        Vec2 drag_direction = particle.velocity.unit_vector() * -1.0;
        float drag_mag = k * particle.velocity.mag_sqaure();
        drage_force = drag_direction * drag_mag;
    }
    return drage_force;
}

Vec2 Force::generate_firction_force(const Particle &particle, float k) {
    Vec2 friction_force = Vec2(0.0, 0.0);

    Vec2 friction_direction = particle.velocity.unit_vector() * (-1.0);
    friction_force = friction_direction * k;
    return friction_force;
}

Vec2 Force::generate_g_force(const Particle &a, const Particle &b, float G,
                             float min_distance, float max_distance) {
    Vec2 d = (b.position - a.position);
    float d_square = d.mag_sqaure();
    // clamp value of distance to allow for some visible effects
    d_square = std::clamp(d_square, min_distance, max_distance);
    Vec2 direction = d.unit_vector();
    float mag = G * (a.mass * b.mass) / d_square;
    Vec2 force = direction * mag;
    return force;
}
