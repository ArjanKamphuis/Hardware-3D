#include "Window.h"

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

Window::Window(int width, int height, const wchar_t* name) noexcept
    : mWidth(width), mHeight(height)
{
    RECT wr = { 100, 100, 0, 0 };
    wr.right = wr.left + width;
    wr.bottom = wr.top + height;
    AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, false);
    mhWnd = CreateWindow(WindowClass::GetName(), name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 
        CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, 
        nullptr, nullptr, WindowClass::GetInstance(), this);
    ShowWindow(mhWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
    DestroyWindow(mhWnd);
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
    }

    return DefWindowProc(hWnd, msg, wParam, lParam);
}
