#include "contact.h"
#include "vec2.h"
#include <algorithm>

void Contact::resolve_penetration() {
    if (a->is_static() && b->is_static()) {
        return;
    }

    // $d_{a} = depth * (m_b / (m_a + m_b))$
    // $d_{b} = depth * (m_a / (m_a + m_b))$
    float da = depth / (a->inv_mass + b->inv_mass) * a->inv_mass;
    float db = depth / (a->inv_mass + b->inv_mass) * b->inv_mass;

    a->position -= normal * da;
    b->position += normal * db;
}

void Contact::resolve_collision() {
    resolve_penetration();

    // elasticity
    float e = std::min(a->restitution, b->restitution);

    const Vec2 v_rel = (a->velocity - b->velocity);
    float v_rel_dot_normal = v_rel.dot(normal);

    const Vec2 impulse_dir = normal;
    const float impulse_mag =
        -(1 + e) * v_rel_dot_normal / (a->inv_mass + b->inv_mass);

    Vec2 j = impulse_dir * impulse_mag;

    a->apply_impulse(j);
    b->apply_impulse(-j);
}
