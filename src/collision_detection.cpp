#include "collision_detection.h"
#include "contact.h"
#include "shape.h"
#include "vec2.h"

bool CollisionDetection::is_colliding(Body *a, Body *b, Contact &contact) {
    bool a_is_circle = a->shape->get_type() == CIRCLE;
    bool b_is_circle = b->shape->get_type() == CIRCLE;
    bool a_is_polygon =
        a->shape->get_type() == POLYGON || a->shape->get_type() == BOX;
    bool b_is_polygon =
        b->shape->get_type() == POLYGON || b->shape->get_type() == BOX;

    if (a_is_circle && b_is_circle) {
        return is_colliding_circle_circle(a, b, contact);
    }

    if (a_is_polygon && b_is_polygon) {
        return is_colliding_polygon_polygon(a, b, contact);
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

bool CollisionDetection::is_colliding_polygon_polygon(Body *a, Body *b,
                                                      Contact &contact) {
    // find separation between a and b, _and_ b and a
    const PolygonShape *a_polygon_shape = (PolygonShape *)a->shape;
    const PolygonShape *b_polygon_shape = (PolygonShape *)b->shape;
    Vec2 a_axis, b_axis;
    Vec2 a_point, b_point;

    float ab_sep =
        a_polygon_shape->find_min_separation(b_polygon_shape, a_axis, a_point);
    if (ab_sep >= 0) {
        return false;
    }

    float ba_sep =
        b_polygon_shape->find_min_separation(a_polygon_shape, b_axis, b_point);
    if (ba_sep >= 0) {
        return false;
    }

    contact.a = a;
    contact.b = b;
    // always pointing from a to b
    if (ab_sep > ba_sep) {
        contact.depth = -ab_sep;
        contact.normal = a_axis.normal();
        contact.start = a_point;
        contact.end = a_point + contact.normal * contact.depth;
    } else {
        contact.depth = -ba_sep;
        contact.normal = -b_axis.normal();
        contact.start = b_point - contact.normal * contact.depth;
        contact.end = b_point;
    }

    return true;
}
