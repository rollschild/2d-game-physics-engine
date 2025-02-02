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

    a->shape->update_vertices(a->rotation, a->position);
    b->shape->update_vertices(b->rotation, b->position);
}

void Contact::resolve_collision() {
    resolve_penetration();

    // elasticity
    float e = std::min(a->restitution, b->restitution);
    // friction
    float f = std::min(a->friction, b->friction);

    Vec2 ra = end - a->position;
    Vec2 rb = start - b->position;
    Vec2 va = a->velocity + Vec2(-a->angular_vel * ra.y, a->angular_vel * ra.x);
    Vec2 vb = b->velocity + Vec2(-b->angular_vel * rb.y, b->angular_vel * rb.x);
    const Vec2 v_rel = va - vb;

    // collision impulse along the normal
    float v_rel_dot_normal = v_rel.dot(normal);
    const Vec2 impulse_dir_normal = normal;
    const float impulse_mag_normal =
        -(1 + e) * v_rel_dot_normal /
        ((a->inv_mass + b->inv_mass) +
         ra.cross(normal) * ra.cross(normal) * a->inv_I +
         rb.cross(normal) * rb.cross(normal) * b->inv_I);
    // impulse should be applied at the point of contact, _not_ on center of
    // mass
    Vec2 jn = impulse_dir_normal * impulse_mag_normal;

    // collision impulse along the tangent
    Vec2 tangent = normal.normal();
    float v_rel_dot_tangent = v_rel.dot(tangent);
    const Vec2 impulse_dir_tan = tangent;
    const float impulse_mag_tan =
        f * -(1 + e) * v_rel_dot_tangent /
        ((a->inv_mass + b->inv_mass) +
         ra.cross(tangent) * ra.cross(tangent) * a->inv_I +
         rb.cross(tangent) * rb.cross(tangent) * b->inv_I);
    Vec2 jt = impulse_dir_tan * impulse_mag_tan;

    Vec2 j = jn + jt;

    a->apply_impulse_at_point(j, ra);
    b->apply_impulse_at_point(-j, rb);
}
