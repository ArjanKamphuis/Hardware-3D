#include "Window.h"

#include "resource.h"
#include "WindowThrowMacros.h"
#include "imgui/imgui_impl_win32.h"

Window::WindowClass Window::WindowClass::mWndClass;
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
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
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

    ImGui_ImplWin32_Init(mhWnd);
    mGfx = std::make_unique<Graphics>(mhWnd);

    RAWINPUTDEVICE rid = {};
    rid.usUsage = 0x02;
    rid.usUsagePage = 0x01;
    if (RegisterRawInputDevices(&rid, 1, sizeof(rid)) == FALSE)
        throw CHWND_LAST_EXCEPT();

    ShowWindow(mhWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
    ImGui_ImplWin32_Shutdown();
    DestroyWindow(mhWnd);
}

void Window::SetTitle(const std::wstring& title)
{
    if (SetWindowText(mhWnd, title.c_str()) == FALSE)
        throw CHWND_LAST_EXCEPT();
}

void Window::EnableCursor() noexcept
{
    mCursorEnabled = true;
    ShowCursor();
    EnableImguiMouse();
    FreeCursor();
}

void Window::DisableCursor() noexcept
{
    mCursorEnabled = false;
    HideCursor();
    DisableImguiMouse();
    ConfineCursor();
}

bool Window::CursorEnabled() const noexcept
{
    return mCursorEnabled;
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

void Window::ConfineCursor() noexcept
{
    RECT rect;
    GetClientRect(mhWnd, &rect);
    MapWindowPoints(mhWnd, nullptr, reinterpret_cast<POINT*>(&rect), 2u);
    ClipCursor(&rect);
}

void Window::FreeCursor() noexcept
{
    ClipCursor(nullptr);
}

void Window::HideCursor() noexcept
{
    while (::ShowCursor(FALSE) >= 0);
}

void Window::ShowCursor() noexcept
{
    while (::ShowCursor(TRUE) < 0);
}

void Window::EnableImguiMouse() noexcept
{
    ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
}

void Window::DisableImguiMouse() noexcept
{
    ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NoMouse;
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
    if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
        return true;

    const ImGuiIO& imio = ImGui::GetIO();

    switch (msg)
    {
    case WM_CLOSE:
        PostQuitMessage(0);
        return 0;
    case WM_KILLFOCUS:
        Keyboard.ClearState();
        break;
    case WM_ACTIVATE:
        if (!mCursorEnabled)
        {
            if (wParam == WA_ACTIVE)
            {
                ConfineCursor();
                HideCursor();
            }
            else if (wParam == WA_INACTIVE)
            {
                FreeCursor();
                ShowCursor();
            }
        }
        break;

    case WM_KEYDOWN:
    case WM_SYSKEYDOWN:
        if (imio.WantCaptureKeyboard)
            break;
        if (!(lParam & 0x40000000) || Keyboard.AutorepeatIsEnabled())
            Keyboard.OnKeyPressed(static_cast<unsigned char>(wParam));
        break;
    case WM_KEYUP:
    case WM_SYSKEYUP:
        if (imio.WantCaptureKeyboard)
            break;
        Keyboard.OnKeyReleased(static_cast<unsigned char>(wParam));
        break;
    case WM_CHAR:
        if (imio.WantCaptureKeyboard)
            break;
        Keyboard.OnChar(static_cast<unsigned char>(wParam));
        break;

    case WM_MOUSEMOVE:
        {
            if (!mCursorEnabled)
            {
                if (!Mouse.IsInWindow())
                {
                    SetCapture(mhWnd);
                    Mouse.OnMouseEnter();
                    HideCursor();
                }
                break;
            }

            if (imio.WantCaptureMouse)
                break;

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
            SetForegroundWindow(mhWnd);
            if (!mCursorEnabled)
            {
                ConfineCursor();
                HideCursor();
            }

            if (imio.WantCaptureMouse)
                break;

            const POINTS pt = MAKEPOINTS(lParam);
            Mouse.OnLeftPressed(pt.x, pt.y);
            break;
        }
    case WM_LBUTTONUP:
        {
            if (imio.WantCaptureMouse)
                break;

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
            if (imio.WantCaptureMouse)
                break;

            const POINTS pt = MAKEPOINTS(lParam);
            Mouse.OnRightPressed(pt.x, pt.y);
            break;
        }
    case WM_RBUTTONUP:
        {
            if (imio.WantCaptureMouse)
                break;

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
            if (imio.WantCaptureMouse)
                break;

            const POINTS pt = MAKEPOINTS(lParam);
            Mouse.OnWheelDelta(pt.x, pt.y, GET_WHEEL_DELTA_WPARAM(wParam));
            break;
        }

    case WM_INPUT:
        {
            if (!Mouse.RawEnabled())
                break;

            UINT size = 0;
            if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) == -1)
                break;

            mRawBuffer.resize(size);
            if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, mRawBuffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
                break;

            const RAWINPUT& ri = reinterpret_cast<const RAWINPUT&>(*mRawBuffer.data());
            if (ri.header.dwType == RIM_TYPEMOUSE && (ri.data.mouse.lLastX != 0 || ri.data.mouse.lLastY != 0))
                Mouse.OnRawDelta(ri.data.mouse.lLastX, ri.data.mouse.lLastY);

            break;
        }

    case WM_SIZE:
        if (wParam == SIZE_RESTORED && mGfx != nullptr)
            mGfx->OnResize(LOWORD(lParam), HIWORD(lParam));
        break;
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
