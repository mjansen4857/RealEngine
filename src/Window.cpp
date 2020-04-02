#include "Window.h"
#include <sstream>
#include "resource.h"

Window::WindowClass Window::WindowClass::windowClass;

Window::WindowClass::WindowClass() noexcept :
        hInstance(GetModuleHandle(nullptr)) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = HandleMsgSetup;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = GetInstance();
    wc.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0));
//    wc.hIcon = static_cast<HICON>(LoadIcon(hInstance, ));
    wc.hCursor = nullptr;
    wc.hbrBackground = nullptr;
    wc.lpszMenuName = nullptr;
    wc.lpszClassName = GetName();
    wc.hIconSm = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0));
    RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass() {
    UnregisterClass(windowClassName, GetInstance());
}

const char* Window::WindowClass::GetName() noexcept {
    return windowClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept {
    return windowClass.hInstance;
}

Window::Window(int width, int height, const char* name) {
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;
    if (FAILED(AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE))) {
        throw DXWINDOW_LAST_EXCEPTION();
    }

    hWindow = CreateWindow(
            WindowClass::GetName(),
            name,
            WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
            CW_USEDEFAULT,
            CW_USEDEFAULT,
            wr.right - wr.left,
            wr.bottom - wr.top,
            nullptr,
            nullptr,
            WindowClass::GetInstance(),
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

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    if (msg == WM_NCCREATE) {
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWindow = static_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWindow, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWindow));
        SetWindowLongPtr(hWindow, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
        return pWindow->HandleMsg(hWindow, msg, wParam, lParam);
    }
    return DefWindowProc(hWindow, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    Window* const pWindow = reinterpret_cast<Window*>(GetWindowLongPtr(hWindow, GWLP_USERDATA));
    return pWindow->HandleMsg(hWindow, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWindow, UINT msg, WPARAM wParam, LPARAM lParam) noexcept {
    switch (msg) {
        case WM_CLOSE:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hWindow, msg, wParam, lParam);
}

Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept :
        RealException(line, file),
        hr(hr) {

}

const char* Window::Exception::what() const noexcept {
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] " << GetErrorCode() << std::endl
        << "[Description] " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept {
    return "RealEngine Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept {
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

HRESULT Window::Exception::GetErrorCode() const noexcept {
    return hr;
}

std::string Window::Exception::GetErrorString() const noexcept {
    return TranslateErrorCode(hr);
}