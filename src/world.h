#ifndef WORLD_H
#define WORLD_H

#include "body.h"
#include "constraint.h"
#include "vec2.h"
#include <vector>

class World {
  private:
    float G = 9.8;
    std::vector<Body *> bodies;
    std::vector<Constraint *> constraints;
    std::vector<Vec2> forces;
    std::vector<float> torques;

  public:
    World(float gravity);
    ~World();

    void add_body(Body *body);
    std::vector<Body *> &get_bodies();

    void add_constraint(Constraint *constraint);
    std::vector<Constraint *> &get_constraints();

    void apply_force(const Vec2 &force);
    void apply_torque(float torque);

    void update(float dt);
    void check_collisions();
};

#endif
