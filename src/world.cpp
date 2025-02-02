#include "world.h"
#include "body.h"
#include "collision_detection.h"
#include "constants.h"
#include "constraint.h"
#include "contact.h"
#include "vec2.h"
#include <iostream>
#include <vector>

World::World(float gravity) {
    G = -gravity;
    std::cout << "World constructor called!" << std::endl;
}

World::~World() {
    for (auto body : bodies) {
        delete body;
    }
    std::cout << "World destructor called!" << std::endl;
}

void World::add_body(Body *body) { bodies.push_back(body); }

std::vector<Body *> &World::get_bodies() { return bodies; }

void World::apply_force(const Vec2 &force) { forces.push_back(force); }
void World::apply_torque(float torque) { torques.push_back(torque); }

/**
 * 1. calculate external forces and acceleration
 * 2. integrate acceleration to find new velocities
 * 3. loop all constraints, solving them
 * 4. integrate velocities to find new positions
 */
void World::update(float dt) {
    for (auto body : bodies) {
        Vec2 weight = Vec2(0.0, body->mass * G * PIXELS_PER_METER);
        body->apply_force(weight);

        for (auto force : forces) {
            body->apply_force(force);
        }
        for (auto torque : torques) {
            body->apply_torque(torque);
        }
    }

    // 1. Integrate all forces (a = F/m)
    for (auto body : bodies) {
        body->integrate_forces(dt);
    }

    // 2. solve all constraints
    for (auto &constraint : constraints) {
        constraint->solve();
    }

    // 3. integrate velocities (update vertices)
    for (auto body : bodies) {
        body->integrate_velocities(dt);
    }

    /*
    for (auto body : bodies) {
        body->update(dt);
    }
    */

    check_collisions();
}

void World::check_collisions() {
    for (size_t i = 0; i <= bodies.size() - 1; i++) {
        for (size_t j = i + 1; j < bodies.size(); j++) {
            Body *a = bodies[i];
            Body *b = bodies[j];
            a->is_colliding = false;
            b->is_colliding = false;

            Contact contact;
            if (CollisionDetection::is_colliding(a, b, contact)) {
                contact.resolve_collision();
            }
        }
    }
}

void World::add_constraint(Constraint *constraint) {
    constraints.push_back(constraint);
}

std::vector<Constraint *> &World::get_constraints() { return constraints; }
