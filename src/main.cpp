#include "app_particle.h"
#include "app_rigid_body.h"

int main(/* int argc, char *args[] */) {
    // AppParticle app;
    AppRigidBody app;
    app.setup();

    while (app.is_running()) {
        app.input();
        app.update();
        app.render();
    }

    app.destroy();

    return 0;
}
