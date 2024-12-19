#ifndef APP_PARTICLE_H
#define APP_PARTICLE_H

#include "graphics.h"
#include "particle.h"
#include "vec2.h"
#include <SDL2/SDL_rect.h>
#include <vector>

class AppParticle {
  private:
    bool running = false;
    std::vector<Particle *> particles;

    Vec2 push_force = Vec2(0, 0); // controlled by keyboard
    Vec2 mouse_cursor = Vec2(0, 0);
    bool left_mouse_button_down = false;

    SDL_Rect liquid;

    Vec2 anchor;
    float rest_chain_length = 15;
    float rest_soft_body_length = 200;
    float k_chain = 300;
    float k_soft_body = 1500;
    const int NUM_CHAINED_PARTICLES = 15;
    const int NUM_SOFT_BODY_PARTICLES = 4;

  public:
    AppParticle() = default;
    ~AppParticle() = default;
    bool is_running();
    void setup();
    void input();
    void update();
    void render();
    void destroy();
};

#endif
