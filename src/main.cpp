#include "app_particle.h"

int main(/* int argc, char *args[] */) {
    AppParticle app;
    app.setup();

    while (app.is_running()) {
        app.input();
        app.update();
        app.render();
    }

    app.destroy();

    return 0;
}
