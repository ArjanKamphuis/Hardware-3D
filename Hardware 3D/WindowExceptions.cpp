#include "Window.h"

#include <sstream>

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

