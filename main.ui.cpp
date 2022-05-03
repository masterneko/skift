#include <karm-ui/button.h>
#include <karm-ui/label.h>
#include <karm-ui/state.h>
#include <karm-ui/window.h>

using namespace Karm::Ui;

int main(int, char **) {
    Ui app{[] {
        window([] {
            auto state = use_state(0);

            label(u8"You, clicked {} times", state.value());
            button(u8"Click me!", [&](Event const &) {
                state.update([](auto &s) {
                    return s + 1;
                });
            });
        });
    }};

    return app.run();
}
