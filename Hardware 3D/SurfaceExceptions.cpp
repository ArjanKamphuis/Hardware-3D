#include "Surface.h"

#include <sstream>

Surface::Exception::Exception(int line, const wchar_t* file, const std::wstring& info) noexcept
	: ChiliException(line, file), mInfo(info)
{
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
	oss << GetType() << std::endl << GetOriginString() << std::endl << L"[Note]" << GetErrorInfo();
	mWideWhatBuffer = oss.str();
}
