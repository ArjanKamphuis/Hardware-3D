#include "Surface.h"

#include <sstream>
#include "Window.h"

Surface::Exception::Exception(int line, const wchar_t* file, const std::wstring& info, std::optional<HRESULT> hr) noexcept
	: ChiliException(line, file), mInfo(L"[Note] " + info)
{
	if (hr)
		mInfo = L"[Error String] " + Window::Exception::TranslateErrorCode(*hr) + L"\n" + mInfo;
}

Surface::Exception::Exception(int line, const wchar_t* file, const std::wstring& filename, const std::wstring& info, std::optional<HRESULT> hr) noexcept
	: ChiliException(line, file)
{
	mInfo = L"[File] " + filename + L"\n[Note] " + info;
	if (hr)
		mInfo = L"[Error String] " + Window::Exception::TranslateErrorCode(*hr) + L"\n" + mInfo;
}

const wchar_t* Surface::Exception::GetType() const noexcept
{
	return L"Chili Surface Exception";
}

std::wstring Surface::Exception::GetErrorInfo() const noexcept
{
	return mInfo;
}

void Surface::Exception::GenerateMessage() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl << GetOriginString() << std::endl << GetErrorInfo();
	mWideWhatBuffer = oss.str();
}
