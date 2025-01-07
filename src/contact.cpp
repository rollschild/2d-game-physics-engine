#include "contact.h"

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
