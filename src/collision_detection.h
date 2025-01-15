#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "body.h"
#include "contact.h"
#include "shape.h"

struct CollisionDetection {
    static bool is_colliding(Body *a, Body *b, Contact &contact);
    static bool is_colliding_circle_circle(Body *a, Body *b, Contact &contact);
    static bool is_colliding_polygon_polygon(Body *a, Body *b,
                                             Contact &contact);
    static bool is_colliding_polygon_circle(Body *polygon, Body *circle,
                                            Contact &contact);
};

#endif
