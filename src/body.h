#ifndef BODY_H
#define BODY_H

#include "shape.h"
#include "vec2.h"

struct Body {
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleartion;

    Vec2 sum_forces;

    float mass;
    float inv_mass;

    Shape *shape = nullptr;

    Body(const Shape &shape, float x, float y, float m);
    ~Body();

    // copy constructor
    Body(const Body &other) = default;

    void apply_force(const Vec2 &force);
    void clear_forces();

    void integrate(float dt);
};

#endif
