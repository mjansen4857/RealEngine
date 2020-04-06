#include "Window.h"
#include <sstream>
#include "resource.h"

Window::WindowClass Window::WindowClass::windowClass;

Window::WindowClass::WindowClass() noexcept : hInstance(GetModuleHandle(nullptr)) {
    WNDCLASSEX wc = {0};
    wc.cbSize = sizeof(wc);
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = handleMsgSetup;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = getInstance();
    wc.hIcon = static_cast<HICON>(LoadImage(hInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0));
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
    if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0) {
        throw RE_WINDOW_LAST_EXCEPTION();
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
        throw RE_WINDOW_LAST_EXCEPTION();
    }

    ShowWindow(hWindow, SW_SHOWDEFAULT);
    gfx = std::make_unique<Graphics>(hWindow);
}

Window::~Window() {
    DestroyWindow(hWindow);
}

void Window::setTitle(const std::string title) {
    if(SetWindowText(hWindow, title.c_str()) == 0){
        throw RE_WINDOW_LAST_EXCEPTION();
    }
}

std::optional<int> Window::processMessages() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            return (int) msg.wParam;
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return {};
}

Graphics& Window::graphics() {
    return *gfx;
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
        case WM_MOUSEMOVE:
        {
            const POINTS point = MAKEPOINTS(lParam);
            if(point.x >=0 && point.x < width && point.y >= 0 && point.y < height){
                mouse.onMouseMove(point.x, point.y);
                if(!mouse.isInWindow()){
                    SetCapture(hWindow);
                    mouse.onMouseEnter();
                }
            }else{
                if(wParam & (MK_LBUTTON | MK_RBUTTON)){
                    mouse.onMouseMove(point.x, point.y);
                }else{
                    ReleaseCapture();
                    mouse.onMouseLeave();
                }
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            const POINTS point = MAKEPOINTS(lParam);
            mouse.onLeftPressed(point.x, point.y);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            const POINTS point = MAKEPOINTS(lParam);
            mouse.onRightPressed(point.x, point.y);
            break;
        }
        case WM_LBUTTONUP:
        {
            const POINTS point = MAKEPOINTS(lParam);
            mouse.onLeftReleased(point.x, point.y);
            break;
        }
        case WM_RBUTTONUP:
        {
            const POINTS point = MAKEPOINTS(lParam);
            mouse.onRightReleased(point.x, point.y);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            const POINTS point = MAKEPOINTS(lParam);
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            mouse.onWheelDelta(point.x, point.y, delta);
            break;
        }
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