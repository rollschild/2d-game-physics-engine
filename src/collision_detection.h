#ifndef COLLISION_DETECTION_H
#define COLLISION_DETECTION_H

#include "body.h"
#include "contact.h"
#include "shape.h"
#include <vector>

struct CollisionDetection {
    static bool is_colliding(Body *a, Body *b, std::vector<Contact> &contacts);
    static bool is_colliding_circle_circle(Body *a, Body *b,
                                           std::vector<Contact> &contact);
    static bool is_colliding_polygon_polygon(Body *a, Body *b,
                                             std::vector<Contact> &contact);
    static bool is_colliding_polygon_circle(Body *polygon, Body *circle,
                                            std::vector<Contact> &contact);
};

#endif
