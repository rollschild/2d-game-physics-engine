#ifndef CONTACT_H
#define CONTACT_H

#include "body.h"
#include "vec2.h"

struct Contact {
    Body *a;
    Body *b;

    Vec2 start;
    Vec2 end;

    Vec2 normal;
    float depth;

    // Contact() = default;
    // ~Contact() = default;

    // copy constructor
    // Contact(const Contact &other) = default;

    // void resolve_penetration();
    // void resolve_collision();
};

#endif
