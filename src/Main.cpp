#include "Window.h"

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
