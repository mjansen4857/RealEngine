#pragma once
#include "RealWin.h"
#include "RealException.h"
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <optional>
#include <memory>

class Window {
public:
    class Exception : public RealException {
        using RealException::RealException;
    public:
        static std::string translateErrorCode(HRESULT hr) noexcept;
    };
    class HRException : public Exception {
    public:
        HRException(int line, const char* file, HRESULT hr) noexcept;
        const char* what() const noexcept override;
        const char* getType() const noexcept override;
        HRESULT getErrorCode() const noexcept;
        std::string getErrorDescription() const noexcept;
    private:
        HRESULT hr;
    };
    class NoGFXException : public Exception {
    public:
        using Exception::Exception;
        const char* getType() const noexcept override;
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
    void setTitle(const std::string title);
    static std::optional<int> processMessages() noexcept;
    Graphics& graphics();

    Keyboard keyboard;
    Mouse mouse;
private:
    static LRESULT CALLBACK handleMsgSetup(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    static LRESULT CALLBACK handleMsgThunk(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
    LRESULT handleMsg(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

    int width;
    int height;
    HWND hWindow;
    std::unique_ptr<Graphics> gfx;
};

#define RE_WINDOW_EXCEPTION(hr) Window::HRException(__LINE__, __FILE__, hr)
#define RE_WINDOW_LAST_EXCEPTION() Window::HRException(__LINE__, __FILE__, GetLastError())
#define RE_WINDOW_NOGFX_EXCEPTION() Window::NoGFXException(__LINE__, __FILE__)