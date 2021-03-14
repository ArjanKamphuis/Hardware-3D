#include "Window.h"

#include <sstream>
#include "resource.h"

Window::WindowClass Window::WindowClass::mWndClass;

Window::Exception::Exception(int line, const wchar_t* file, HRESULT hr) noexcept
    : ChiliException(line, file), mHR(hr)
{
}

const wchar_t* Window::Exception::GetType() const noexcept
{
    return L"Chili Window Exception";
}

std::wstring Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
    wchar_t* pMsgBuf = nullptr;
    DWORD nMsgLength = FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), reinterpret_cast<LPWSTR>(&pMsgBuf), 0, nullptr);
    if (nMsgLength == 0)
        return L"Unidentified error code.\r\n";

    std::wstring errorString = pMsgBuf;
    LocalFree(pMsgBuf);
    return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
    return mHR;
}

std::wstring Window::Exception::GetErrorString() const noexcept
{
    return TranslateErrorCode(mHR);
}

void Window::Exception::GenerateMessage() const noexcept
{
    std::wstringstream oss;
    oss << GetType() << std::endl << "[Error Code] " << GetErrorCode() << std::endl << "[Description] " << GetErrorString() << std::endl << GetOriginString();
    mWideWhatBuffer = oss.str();
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
    RECT wr = { 100, 100, 0, 0 };
    wr.right = wr.left + width;
    wr.bottom = wr.top + height;
    if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == FALSE)
        throw CHWND_LAST_EXCEPT();

    mhWnd = CreateWindow(WindowClass::GetName(), name, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 
        CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top, 
        nullptr, nullptr, WindowClass::GetInstance(), this);

    if (mhWnd == nullptr)
        throw CHWND_LAST_EXCEPT();

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
