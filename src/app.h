#ifndef APP_H
#define APP_H

#include "graphics.h"
#include "particle.h"
#include "vec2.h"
#include <SDL2/SDL_rect.h>
#include <vector>

class App {
  private:
    bool running = false;
    std::vector<Particle *> particles;

    Vec2 push_force = Vec2(0, 0); // controlled by keyboard
    Vec2 mouse_cursor = Vec2(0, 0);
    bool left_mouse_button_down = false;

    SDL_Rect liquid;

  public:
    App() = default;
    ~App() = default;
    bool is_running();
    void setup();
    void input();
    void update();
    void render();
    void destroy();
};

#endif
