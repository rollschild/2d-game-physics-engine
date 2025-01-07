#include "collision_detection.h"
#include "contact.h"
#include "shape.h"
#include "vec2.h"

bool CollisionDetection::is_colliding(Body *a, Body *b, Contact &contact) {
    bool a_is_circle = a->shape->get_type() == CIRCLE;
    bool b_is_circle = b->shape->get_type() == CIRCLE;

    if (a_is_circle && b_is_circle) {
        return is_colliding_circle_circle(a, b, contact);
    }
    return false;
}

bool CollisionDetection::is_colliding_circle_circle(Body *a, Body *b,
                                                    Contact &contact) {
    CircleShape *a_shape = (CircleShape *)a->shape;
    CircleShape *b_shape = (CircleShape *)b->shape;

    const Vec2 ab = b->position - a->position;
    const auto sum_radius = a_shape->radius + b_shape->radius;

    bool is_colliding = ab.mag_sqaure() <= sum_radius * sum_radius;
    if (!is_colliding) {
        return false;
    }

    // compute Contact information
    contact.a = a;
    contact.b = b;
    contact.normal = ab;
    contact.normal.normalize();
    contact.start = b->position - contact.normal * b_shape->radius;
    contact.end = a->position + contact.normal * a_shape->radius;
    contact.depth = (contact.end - contact.start).mag();

    return true;
}
