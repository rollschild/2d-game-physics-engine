#ifndef BODY_H
#define BODY_H

#include "shape.h"
#include "vec2.h"
#include <SDL2/SDL.h>
// #include <SDL2/SDL_image.h>
#include <SDL2/SDL_render.h>

struct Body {
    // bool is_colliding = false;

    // linear motion
    Vec2 position;
    Vec2 velocity;
    Vec2 acceleartion;

    // Angular motion
    float rotation;
    float angular_vel;
    float angular_acc;

    // forces and torque
    Vec2 sum_forces;
    float sum_torque;

    // mass and moment of inertia
    float mass;
    float inv_mass;
    float I;
    float inv_I;

    // coefficient of restitution (elasticity)
    float restitution;
    // coefficient of friction
    float friction;

    Shape *shape = nullptr;

    // pointer to SDL texture
    SDL_Texture *texture = nullptr;

    Body(const Shape &shape, float x, float y, float m);
    ~Body();

    // copy constructor
    Body(const Body &other) = default;

    void apply_force(const Vec2 &force);
    void apply_torque(const float torque);
    void clear_forces();
    void clear_torque();

    void integrate_linear(float dt);
    void integrate_angular(float dt);

    void update(float dt);

    void set_texture(const char *texture_file_name);

    Vec2 localspace_to_worldspace(const Vec2 &point) const;
    Vec2 worldspace_to_localspace(const Vec2 &point) const;

    bool is_static() const;

    /**
     * Linear impulse, applied at center of mass
     */
    void apply_impulse_linear(const Vec2 &j);
    /**
     * Angular impulse, applied at center of mass
     */
    void apply_impulse_angular(const float j);
    /**
     * Both linear and angular impulse, applied at point of contact
     */
    void apply_impulse_at_point(const Vec2 &j, const Vec2 &r);

    void integrate_forces(const float dt);
    void integrate_velocities(const float dt);
};

#endif
