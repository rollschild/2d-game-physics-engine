#include "world.h"
#include "body.h"
#include "collision_detection.h"
#include "constants.h"
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

    for (auto body : bodies) {
        body->update(dt);
    }

    for (int n = 0; n < 10; n++) {
        // trying to fix unstable jittering
        // lowers our FPS
        check_collisions();
    }
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
