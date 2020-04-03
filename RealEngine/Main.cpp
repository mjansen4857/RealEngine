#include "Window.h"
#include <sstream>

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    try {
        Window window(1280, 720, "RealEngine");

        MSG msg;
        BOOL result;
        while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);

            if(window.keyboard.isKeyPressed(VK_MENU)){
                MessageBox(nullptr, "Alt key pressed", "Keyboard Thing", MB_OK | MB_ICONEXCLAMATION);
            }

            if(window.mouse.isRightPressed()){
                MessageBox(nullptr, "Right mouse button pressed", "Mouse Thing", MB_OK | MB_ICONEXCLAMATION);
            }

            while(!window.mouse.isBufferEmpty()){
                const auto e = window.mouse.read();
                if(e.getType() == Mouse::Event::Type::Move){
                    std::ostringstream oss;
                    oss << "RealEngine Window | Mouse Pos: (" << e.getXPos() << ", " << e.getYPos() << ")";
                    window.setTitle(oss.str());
                }
            }
        }

        if (result == -1) {
            return -1;
        }

        return msg.wParam;
    } catch (const RealException& e) {
        MessageBox(nullptr, e.what(), e.getType(), MB_OK | MB_ICONERROR);
    } catch (const std::exception& e) {
        MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONERROR);
    } catch (...) {
        MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONERROR);
    }

    return -1;
}
