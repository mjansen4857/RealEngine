#include "App.h"

int r = 42;
int g = 42;
int b = 42;

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
    /*if (r > 0 && b == 0) {
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
    }*/

    //const float r = sin(timer.get()) / 2.0f + 0.5f;
    //const float g = sin(timer.get() / 2) / 2.0f + 0.5f;
    //const float b = sin(timer.get() / 4) / 2.0f + 0.5f;
    window.graphics().clearBuffer(r / 255.0f, g / 255.0f, b / 255.0f);
    float mouseX = window.mouse.getXPos() / 640.0f - 1.0f;
    float mouseY = -window.mouse.getYPos() / 480.0f + 1.0f;
    window.graphics().drawTestTriangle(timer.get(), mouseX, mouseY);
    window.graphics().presentFrame();
}