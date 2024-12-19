#include "app_particle.h"
#include "constants.h"
#include "force.h"
#include "graphics.h"
#include "particle.h"
#include "vec2.h"
#include <SDL2/SDL_events.h>
#include <SDL2/SDL_keycode.h>
#include <SDL2/SDL_mouse.h>
#include <SDL2/SDL_timer.h>
#include <vector>

bool AppParticle::is_running() { return running; }

void setup_spring_chain(std::vector<Particle *> &particles, const Vec2 &anchor,
                        float len, int num) {
    for (int i = 0; i < num; i++) {
        Particle *bob = new Particle(anchor.x, anchor.y + (i * len), 2.0);

        bob->radius = 6;
        particles.push_back(bob);
    }
}

void setup_spring_soft_body(std::vector<Particle *> &particles) {
    Particle *a = new Particle(100, 100, 1.0);
    Particle *b = new Particle(300, 100, 1.0);
    Particle *c = new Particle(300, 300, 1.0);
    Particle *d = new Particle(100, 300, 1.0);

    a->radius = 6;
    b->radius = 6;
    c->radius = 6;
    d->radius = 6;

    particles.push_back(a);
    particles.push_back(b);
    particles.push_back(c);
    particles.push_back(d);
}

/**
 * Setup function (executed once in the beginning of the simulation)
 */
void AppParticle::setup() {
    running = Graphics::open_window();

    Particle *small_planet = new Particle(200, 200, 1.0);
    small_planet->radius = 6;
    // particles.push_back(small_planet);

    Particle *big_planet = new Particle(500, 500, 20.0);
    big_planet->radius = 20;
    // particles.push_back(big_planet);

    liquid.x = 0;
    liquid.y = Graphics::height() / 2;
    liquid.w = Graphics::width();
    liquid.h = Graphics::height() / 2;

    anchor = Vec2(Graphics::width() / 2.0, 30);
    // setup_spring_chain(particles, anchor, rest_chain_length,
    // NUM_CHAINED_PARTICLES);
    setup_spring_soft_body(particles);
}

/**
 * Input processing
 */
void AppParticle::input() {
    SDL_Event event;

    // poll events
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
        case SDL_QUIT:
            running = false;
            break;
        case SDL_KEYDOWN:
            if (event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
            if (event.key.keysym.sym == SDLK_UP) {
                push_force.y = -50 * PIXELS_PER_METER;
            }
            if (event.key.keysym.sym == SDLK_RIGHT) {
                push_force.x = 50 * PIXELS_PER_METER;
            }
            if (event.key.keysym.sym == SDLK_DOWN) {
                push_force.y = 50 * PIXELS_PER_METER;
            }
            if (event.key.keysym.sym == SDLK_LEFT) {
                push_force.x = -50 * PIXELS_PER_METER;
            }
            break;
        case SDL_KEYUP:
            if (event.key.keysym.sym == SDLK_UP ||
                event.key.keysym.sym == SDLK_DOWN) {
                push_force.y = 0;
            }
            if (event.key.keysym.sym == SDLK_RIGHT ||
                event.key.keysym.sym == SDLK_LEFT) {
                push_force.x = 0;
            }
            break;
        case SDL_MOUSEMOTION:
            mouse_cursor.x = event.motion.x;
            mouse_cursor.y = event.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            /*
            if (event.button.button == SDL_BUTTON_LEFT) {
                int x, y;
                SDL_GetMouseState(&x, &y);
                Particle *particle = new Particle(x, y, 1.0);
                particle->radius = 5;
                particles.push_back(particle);
            }
            */
            if (!left_mouse_button_down &&
                event.button.button == SDL_BUTTON_LEFT) {
                left_mouse_button_down = true;
                int x, y;
                SDL_GetMouseState(&x, &y);
                mouse_cursor.x = x;
                mouse_cursor.y = y;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (left_mouse_button_down &&
                event.button.button == SDL_BUTTON_LEFT) {
                left_mouse_button_down = false;
                // int last_particle = NUM_CHAINED_PARTICLES - 1;
                int last_particle = NUM_SOFT_BODY_PARTICLES - 1;
                Vec2 impluse_direction =
                    (particles[last_particle]->position - mouse_cursor)
                        .unit_vector();
                float impulse_mag =
                    (particles[last_particle]->position - mouse_cursor).mag() *
                    5.0;
                particles[last_particle]->velocity =
                    impluse_direction * impulse_mag;
            }
            break;
        }
    }
}

void apply_spring_soft_body_forces(std::vector<Particle *> &particles,
                                   float len, float k) {
    Vec2 ab =
        Force::generate_spring_force(*particles[0], *particles[1], len, k);
    particles[0]->apply_force(ab);
    particles[1]->apply_force(-ab);

    Vec2 bc =
        Force::generate_spring_force(*particles[1], *particles[2], len, k);
    particles[1]->apply_force(bc);
    particles[2]->apply_force(-bc);

    Vec2 cd =
        Force::generate_spring_force(*particles[2], *particles[3], len, k);
    particles[2]->apply_force(cd);
    particles[3]->apply_force(-cd);

    Vec2 da =
        Force::generate_spring_force(*particles[3], *particles[0], len, k);
    particles[3]->apply_force(da);
    particles[0]->apply_force(-da);

    Vec2 ac =
        Force::generate_spring_force(*particles[0], *particles[2], len, k);
    particles[0]->apply_force(ac);
    particles[2]->apply_force(-ac);

    Vec2 bd =
        Force::generate_spring_force(*particles[1], *particles[3], len, k);
    particles[1]->apply_force(bd);
    particles[3]->apply_force(-bd);
}

void apply_spring_chain_forces(std::vector<Particle *> &particles, Vec2 &anchor,
                               int num, float len, float k) {

    // attach anchor head with a spring
    Vec2 spring_force =
        Force::generate_spring_force(*particles[0], anchor, len, k);
    particles[0]->apply_force(spring_force);

    for (int i = 1; i < num; i++) {
        int curr = i;
        int prev = i - 1;
        Vec2 sf = Force::generate_spring_force(*particles[curr],
                                               *particles[prev], len, k);
        particles[curr]->apply_force(sf);
        particles[prev]->apply_force(-sf);
    }
}

/**
 * Update function (called several times per second to update objects)
 */
void AppParticle::update() {
    // Check if we are too fast; if so, wait for some milliseconds,
    // until we reach the MS_PER_FRAME
    static int time_previous_frame;
    // SDL_GetTicks(): current total number of milliseconds that our SDL app has
    // been running
    int time_to_wait = MS_PER_FRAME - (SDL_GetTicks() - time_previous_frame);
    // we want to make sure MS_PER_FRAME has passed
    if (time_to_wait > 0) {
        SDL_Delay(time_to_wait);
    }

    // delta time: difference between the current frame and the last frame (in
    // seconds)
    // we want to move _per second_, not _per frame_
    // **frame rate independent movement**
    //  (current time in ms of this frame - time in ms of previous frame)
    float delta_time = (SDL_GetTicks() - time_previous_frame) / 1000.0f;
    // protect delta_time from consuming large values that could potentially
    // mess up the execution
    // e.g. when debugging
    if (delta_time > 0.016) {
        delta_time = 0.016;
    }

    time_previous_frame = SDL_GetTicks();

    // how many pixels to move _per second_

    // apply g force
    /*
    Vec2 g_force =
        Force::generate_g_force(*particles[0], *particles[1], 1000.0, 5, 100);
    particles[0]->apply_force(g_force);
    particles[1]->apply_force(-g_force);
    */

    if (particles.size() > 0) {
        particles[particles.size() - 1]->apply_force(push_force);
    }

    for (auto particle : particles) {
        // Vec2 wind = Vec2(1.0 * PIXELS_PER_METER, 0.0);
        // particle->apply_force(wind);

        Vec2 weight = Vec2(0.0, particle->mass * 9.8 * PIXELS_PER_METER);
        particle->apply_force(weight);

        Vec2 friction = Force::generate_friction_force(*particle, 20);
        particle->apply_force(friction);

        Vec2 drag = Force::generate_drag_force(*particle, 0.002);
        particle->apply_force(drag);

        // apply drag forces when in liquid
        // if (particle->position.y >= liquid.y) {
        // Vec2 drag = Force::generate_drag_force(*particle, 0.03);
        // particle->apply_force(drag);
        // }
    }

    // apply_spring_chain_forces(particles, anchor, NUM_CHAINED_PARTICLES,
    // rest_chain_length, k_chain);
    apply_spring_soft_body_forces(particles, rest_soft_body_length,
                                  k_soft_body);

    for (auto particle : particles) {
        // Integrate the acceleration and velocity to estimate the new position
        particle->integrate(delta_time);
    }

    for (auto particle : particles) {
        if (particle->position.x - particle->radius <= 0) {
            // left border reached
            particle->position.x = particle->radius;
            particle->velocity.x *= -0.9;
        } else if (particle->position.x + particle->radius >=
                   Graphics::width()) {
            // reached right border
            particle->position.x = Graphics::width() - particle->radius;
            particle->velocity.x *= -0.9;
        }
        if (particle->position.y - particle->radius <= 0) {
            // top border reached
            particle->position.y = particle->radius;
            particle->velocity.y *= -0.9;
        } else if (particle->position.y + particle->radius >=
                   Graphics::height()) {
            // reached bottom border
            particle->position.y = Graphics::height() - particle->radius;
            particle->velocity.y *= -0.9;
        }
    }
}

void render_spring_soft_body(const std::vector<Particle *> &particles) {
    Graphics::draw_line(particles[0]->position.x, particles[0]->position.y,
                        particles[1]->position.x, particles[1]->position.y,
                        0xFF313131);
    Graphics::draw_line(particles[1]->position.x, particles[1]->position.y,
                        particles[2]->position.x, particles[2]->position.y,
                        0xFF313131);
    Graphics::draw_line(particles[2]->position.x, particles[2]->position.y,
                        particles[3]->position.x, particles[3]->position.y,
                        0xFF313131);
    Graphics::draw_line(particles[3]->position.x, particles[3]->position.y,
                        particles[0]->position.x, particles[0]->position.y,
                        0xFF313131);
    Graphics::draw_line(particles[0]->position.x, particles[0]->position.y,
                        particles[2]->position.x, particles[2]->position.y,
                        0xFF313131);
    Graphics::draw_line(particles[1]->position.x, particles[1]->position.y,
                        particles[3]->position.x, particles[3]->position.y,
                        0xFF313131);

    Graphics::draw_fill_circle(particles[0]->position.x,
                               particles[0]->position.y, particles[0]->radius,
                               0xFFEEBB00);
    Graphics::draw_fill_circle(particles[1]->position.x,
                               particles[1]->position.y, particles[1]->radius,
                               0xFFEEBB00);
    Graphics::draw_fill_circle(particles[2]->position.x,
                               particles[2]->position.y, particles[2]->radius,
                               0xFFEEBB00);
    Graphics::draw_fill_circle(particles[3]->position.x,
                               particles[3]->position.y, particles[3]->radius,
                               0xFFEEBB00);
}

void render_spring_chain(const std::vector<Particle *> &particles,
                         const Vec2 &anchor, int num) {
    // draw anchor and sprint to the first bob
    Graphics::draw_fill_circle(anchor.x, anchor.y, 5, 0xFF001155);
    Graphics::draw_line(anchor.x, anchor.y, particles[0]->position.x,
                        particles[0]->position.y, 0xFF313131);

    // draw all springs from one particle to the next
    for (int i = 0; i < num - 1; i++) {
        int curr = i;
        int next = i + 1;
        Graphics::draw_line(particles[curr]->position.x,
                            particles[curr]->position.y,
                            particles[next]->position.x,
                            particles[next]->position.y, 0xFF313131);
    }

    // draw all bob particles
    for (auto particle : particles) {
        Graphics::draw_fill_circle(particle->position.x, particle->position.y,
                                   particle->radius, 0xFFEEBB00);
    }
}

/**
 * Render function (called several times per second to draw objects)
 */
void AppParticle::render() {
    // `FF` - full opacity, no transparency
    Graphics::clear_screen(0xFF056263);

    // draw liquid
    // Graphics::draw_fill_rect(liquid.x + liquid.w / 2, liquid.y + liquid.h /
    // 2, liquid.w, liquid.h, 0xFF6E3713);

    if (left_mouse_button_down) {
        // int last_particle = NUM_CHAINED_PARTICLES - 1;
        int last_particle = NUM_SOFT_BODY_PARTICLES - 1;
        Graphics::draw_line(particles[last_particle]->position.x,
                            particles[last_particle]->position.y,
                            mouse_cursor.x, mouse_cursor.y, 0xFF0000FF);
    }

    /*
    for (auto particle : particles) {

        Graphics::draw_fill_circle(particle->position.x, particle->position.y,
                                   particle->radius,
                                   0xFFFFFFFF); // all white circle
    }
    */
    /*
    Graphics::draw_fill_circle(particles[0]->position.x,
                               particles[0]->position.y, particles[0]->radius,
                               0xFFAA3300);
    Graphics::draw_fill_circle(particles[1]->position.x,
                               particles[1]->position.y, particles[1]->radius,
                               0xFF00FFFF);
    */
    // render_spring_chain(particles, anchor, NUM_CHAINED_PARTICLES);

    render_spring_soft_body(particles);

    Graphics::render_frame();
}

/**
 * Destroy function to delete objects and close the window
 */
void AppParticle::destroy() {
    for (auto particle : particles) {
        delete particle;
    }
    Graphics::close_window();
}
