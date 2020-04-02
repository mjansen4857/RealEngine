#pragma once
#include "RealWin.h"
#include "RealException.h"

class Window {
public:
    class Exception : public RealException {
    public:
        Exception(int line, const char* file, HRESULT hr) noexcept;
        const char* what() const noexcept override;
        const char* getType() const noexcept override;
        static std::string translateErrorCode(HRESULT hr) noexcept;
        HRESULT getErrorCode() const noexcept;
        std::string getErrorString() const noexcept;
    private:
        HRESULT hr;
    };
private:
    class WindowClass {
    public:
        static const char* getName() noexcept;
        static HINSTANCE getInstance() noexcept;
    private:
        WindowClass() noexcept;
        ~WindowClass();
        WindowClass(const WindowClass&) = delete;
        WindowClass& operator = (const WindowClass&) = delete;
        static constexpr const char* windowClassName = "RealEngine Window";
        static WindowClass windowClass;
        HINSTANCE hInstance;
    };
public:
    Window(int width, int height, const char* name);
    ~Window();
    Window(const Window&) = delete;
    Window& operator = (const Window&) = delete;
private:
    static LRESULT CALLBACK handleMsgSetup(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK handleMsgThunk(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT handleMsg(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    int width;
    int height;
    HWND hWindow;
};

#define DXWINDOW_EXCEPTION(hr) Window::Exception(__LINE__, __FILE__, hr)
#define DXWINDOW_LAST_EXCEPTION() Window::Exception(__LINE__, __FILE__, GetLastError())