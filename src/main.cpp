#include "Window.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    try {
        Window window(1280, 720, "RealEngine");

        MSG msg;
        BOOL result;
        while ((result = GetMessage(&msg, nullptr, 0, 0)) > 0) {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (result == -1) {
            return -1;
        }

        return msg.wParam;
    } catch (const RealException& e) {
        MessageBox(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    } catch (const std::exception& e) {
        MessageBox(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    } catch (...) {
        MessageBox(nullptr, "No details available", "Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
    }

    return -1;
}