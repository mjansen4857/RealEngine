#include "App.h"

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
    
}