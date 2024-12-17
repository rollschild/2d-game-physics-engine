#include "force.h"
#include "particle.h"
#include "vec2.h"

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
