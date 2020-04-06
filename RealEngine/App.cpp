#include "App.h"

int r = 255;
int g = 0;
int b = 0;

App::App() : window(1280, 720, "RealEngine") {

}

int App::start() {
    while (true) {
        if (const auto ecode = Window::processMessages()) {
            return *ecode;
        }

        update();
    }
}

void App::update() {
    if (r > 0 && b == 0) {
        r--;
        g++;
    }

    if (g > 0 && r == 0) {
        g--;
        b++;
    }

    if (b > 0 && g == 0) {
        r++;
        b--;
    }

    //const float r = sin(timer.get()) / 2.0f + 0.5f;
    //const float g = sin(timer.get() / 2) / 2.0f + 0.5f;
    //const float b = sin(timer.get() / 4) / 2.0f + 0.5f;
    window.graphics().clearBuffer(r / 255.0f, g / 255.0f, b / 255.0f);

    window.graphics().presentFrame();
}