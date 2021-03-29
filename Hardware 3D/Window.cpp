#include "Window.h"

#include <sstream>
#include "resource.h"

Window::WindowClass Window::WindowClass::mWndClass;

std::wstring Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
    wchar_t* pMsgBuf = nullptr;
    const DWORD nMsgLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr);
    if (nMsgLength == 0)
        return L"Unidentified error code.";

    std::wstring errorString = pMsgBuf;
    LocalFree(pMsgBuf);

    // delete the /r/n nasty line
    errorString.erase(std::prev(errorString.end(), 2), errorString.end());
    return errorString;
}

Window::HrException::HrException(int line, const wchar_t* file, HRESULT hr) noexcept
    : Exception(line, file), mHR(hr)
{
}

const wchar_t* Window::HrException::GetType() const noexcept
{
    return L"Chili Window Exception";
}

HRESULT Window::HrException::GetErrorCode() const noexcept
{
    return mHR;
}

std::wstring Window::HrException::GetErrorDescription() const noexcept
{
    return TranslateErrorCode(mHR);
}

void Window::HrException::GenerateMessage() const noexcept
{
    std::wstringstream oss;
    oss << GetType() << std::endl << "[Error Code] " << GetErrorCode() << std::endl << "[Description] " << GetErrorDescription() << std::endl << GetOriginString();
    mWideWhatBuffer = oss.str();
}

const wchar_t* Window::NoGfxException::GetType() const noexcept
{
    return L"Chili Window Exception [No Graphics]";
}

const wchar_t* Window::WindowClass::GetName() noexcept
{
    return mWndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return mWndClass.mhInstance;
}

Window::WindowClass::WindowClass() noexcept
    : mhInstance(GetModuleHandle(nullptr))
{
    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(wc);
    wc.hIcon = static_cast<HICON>(LoadImage(mhInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 32, 32, 0));
    wc.hIconSm = static_cast<HICON>(LoadImage(mhInstance, MAKEINTRESOURCE(IDI_APPICON), IMAGE_ICON, 16, 16, 0));
    wc.hInstance = GetInstance();
    wc.lpfnWndProc = HandleMsgSetup;
    wc.lpszClassName = GetName();
    wc.style = CS_OWNDC;
    RegisterClassEx(&wc);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(mWndClassName, GetInstance());
}

Window::Window(int width, int height, const wchar_t* name)
    : mWidth(width), mHeight(height)
{
    if ((mhWnd = CreateWindow(WindowClass::GetName(), name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 0, 0, 0, 0, nullptr, nullptr, WindowClass::GetInstance(), this)) == nullptr)
        throw CHWND_LAST_EXCEPT();
    AdjustAndCenterWindow();

    mGfx = std::make_unique<Graphics>(mhWnd);

    ShowWindow(mhWnd, SW_SHOWDEFAULT);    
}

Window::~Window()
{
    DestroyWindow(mhWnd);
}

void Window::SetTitle(const std::wstring& title)
{
    if (SetWindowText(mhWnd, title.c_str()) == FALSE)
        throw CHWND_LAST_EXCEPT();
}

std::optional<int> Window::ProcessMessages() noexcept
{
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
    {
        if (msg.message == WM_QUIT)
            return static_cast<int>(msg.wParam);

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return {};
}

Graphics& Window::Gfx() const
{
    if (mGfx == nullptr)
        throw CHWND_NOGFX_EXCEPT();
    return *mGfx;
}

void Window::AdjustAndCenterWindow()
{
    WINDOWINFO wi = {};
    wi.cbSize = sizeof(wi);
    if (GetWindowInfo(mhWnd, &wi) == FALSE)
        throw CHWND_LAST_EXCEPT();

    RECT wr = { 0, 0, mWidth, mHeight };
    if (AdjustWindowRect(&wr, wi.dwStyle, FALSE) == FALSE)
        throw CHWND_LAST_EXCEPT();

    const int clientWidth = wr.right - wr.left;
    const int clientHeight = wr.bottom - wr.top;

    MONITORINFO mi = {};
    mi.cbSize = sizeof(mi);
    if (GetMonitorInfo(MonitorFromWindow(mhWnd, MONITOR_DEFAULTTONEAREST), &mi) == FALSE)
        throw CHWND_LAST_EXCEPT();
    if (SetWindowPos(mhWnd, nullptr, (mi.rcMonitor.right - clientWidth) / 2, (mi.rcMonitor.bottom - clientHeight) / 2, clientWidth, clientHeight, 0) == FALSE)
        throw CHWND_LAST_EXCEPT();
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    if (msg == WM_NCCREATE)
    {
        const CREATESTRUCT* const pCreate = reinterpret_cast<CREATESTRUCT*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
        return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    return pWnd->HandleMsg(hWnd, msg, wParam, lParam);
}

LRESULT Window::HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_KILLFOCUS:
        Keyboard.ClearState();
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (!(lParam & 0x40000000) || Keyboard.AutorepeatIsEnabled())
            Keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        Keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
        break;
    case WM_CHAR:
        Keyboard.OnChar(static_cast<unsigned char>(wParam));
        break;

    case WM_MOUSEMOVE:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            if (pt.x >= 0 && pt.x < mWidth && pt.y >= 0 && pt.y < mHeight)
            {
                Mouse.OnMouseMove(pt.x, pt.y);
                if (!Mouse.IsInWindow())
                {
                    SetCapture(mhWnd);
                    Mouse.OnMouseEnter();
                }
            }
            else
            {
                if (Mouse.LeftIsPressed() || Mouse.RightIsPressed())
                    Mouse.OnMouseMove(pt.x, pt.y);
                else
                {
                    ReleaseCapture();
                    Mouse.OnMouseLeave();
                }
            }            
            break;
        }
    case WM_LBUTTONDOWN:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            Mouse.OnLeftPressed(pt.x, pt.y);
            break;
        }
    case WM_LBUTTONUP:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            Mouse.OnLeftReleased(pt.x, pt.y);;
            if (pt.x < 0 || pt.x >= mWidth || pt.y < 0 || pt.y >= mHeight)
            {
                ReleaseCapture();
                Mouse.OnMouseLeave();
            }
            break;
        }
    case WM_RBUTTONDOWN:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            Mouse.OnRightPressed(pt.x, pt.y);
            break;
        }
    case WM_RBUTTONUP:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            Mouse.OnRightReleased(pt.x, pt.y);;
            if (pt.x < 0 || pt.x >= mWidth || pt.y < 0 || pt.y >= mHeight)
            {
                ReleaseCapture();
                Mouse.OnMouseLeave();
            }
            break;
        }
    case WM_MOUSEWHEEL:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            Mouse.OnWheelDelta(pt.x, pt.y, GET_WHEEL_DELTA_WPARAM(wParam));
            break;
        }
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
