#include "Window.h"
#include <sstream>
#include "resource.h"

Window::WindowClass Window::WindowClass::windowClass;

Window::WindowClass::WindowClass() noexcept :
        hInstance(GetModuleHandle(nullptr)) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = handleMsgSetup;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = getInstance();
    wc.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0));
//    wc.hIcon = static_cast<HICON>(LoadIcon(hInstance, ));
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = getName();
    wc.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0));
    RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() {
    UnregisterClass(windowClassName, getInstance());
}

const char* Window::WindowClass::getName() noexcept {
    return windowClassName;
}

HINSTANCE Window::WindowClass::getInstance() noexcept {
    return windowClass.hInstance;
}

Window::Window(int width, int height, const char* name) : width(width), height(height) {
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;
    if (FAILED(AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))) {
        throw DXWINDOW_LAST_EXCEPTION();
    }

    hWindow = CreateWindow(
            WindowClass::getName(),
            name,
            WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            wr.right - wr.left,
            wr.bottom - wr.top,
            nullptr,
            nullptr,
            WindowClass::getInstance(),
            this
    );
    if (hWindow == nullptr) {
        throw DXWINDOW_LAST_EXCEPTION();
    }

    ShowWindow(hWindow, SW_SHOWDEFAULT);
}

Window::~Window() {
    DestroyWindow(hWindow);
}

LRESULT CALLBACK Window::handleMsgSetup(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    if (msg == WM_NCCREATE) {
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWindow = static_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
        SetWindowLongPtr(hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::handleMsgThunk));
        return pWindow->handleMsg(hWindow, msg, wParam, lParam);
    }
    return DefWindowProc(hWindow, msg, wParam, lParam);
}

LRESULT CALLBACK Window::handleMsgThunk(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWindow, GWLP_USERDATA));
    return pWindow->handleMsg(hWindow, msg, wParam, lParam);
}

LRESULT Window::handleMsg(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    switch (msg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
        case WM_KILLFOCUS:
            // Clear key states on focus loss to prevent input getting stuck
            keyboard.clearState();
            break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
            // Weird stuff for windows auto repeating keypresses when held
            if(!(lParam & 0x40000000) || keyboard.isAutorepeatEnabled()) {
                keyboard.onKeyPressed(static_cast<unsigned char>(wParam));
            }
            break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
            keyboard.onKeyReleased(static_cast<unsigned char>(wParam));
            break;
        case WM_CHAR:
            keyboard.onChar(static_cast<unsigned char>(wParam));
            break;
    }
    return DefWindowProc(hWindow, msg, wParam, lParam);
}

Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept :
        RealException(line, file),
        hr(hr) {

}

const char* Window::Exception::what() const noexcept {
    std::ostringstream oss;
    oss << getType() << std::endl
        << "[Error Code] " << getErrorCode() << std::endl
        << "[Description] " << getErrorString() << std::endl
        << getOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Window::Exception::getType() const noexcept {
    return "RealEngine Window Exception";
}

std::string Window::Exception::translateErrorCode(HRESULT hr) noexcept {
    char* msgBuf = nullptr;
    DWORD msgLen = FormatMessage(
            FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
            nullptr,
            hr,
            MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
            reinterpret_cast<LPSTR>(&msgBuf),
            0,
            nullptr
    );

    if (msgLen == 0) {
        return "Unidentified Error Code";
    }

    std::string errorString = msgBuf;
    LocalFree(msgBuf);
    return errorString;
}

HRESULT Window::Exception::getErrorCode() const noexcept {
    return hr;
}

std::string Window::Exception::getErrorString() const noexcept {
    return translateErrorCode(hr);
}