#include "ChiliException.h"

#include <sstream>

ChiliException::ChiliException(int line, const wchar_t* file) noexcept
	: mLine(line), mFile(file)
{
}

const char* ChiliException::what() const noexcept
{
	GenerateMessage();

	char s[1024] = {};
	size_t i = 0;
	wcstombs_s(&i, s, mWideWhatBuffer.size() + 1, mWideWhatBuffer.c_str(), mWideWhatBuffer.size());
	mWhatBuffer.assign(s);

	return mWhatBuffer.c_str();
}

const wchar_t* ChiliException::GetFullMessage() const noexcept
{
	GenerateMessage();
	return mWideWhatBuffer.c_str();
}

const wchar_t* ChiliException::GetType() const noexcept
{
	return L"Chili Exception";
}

int ChiliException::GetLine() const noexcept
{
	return mLine;
}

const std::wstring& ChiliException::GetFile() const noexcept
{
	return mFile;
}

std::wstring ChiliException::GetOriginString() const noexcept
{
	std::wostringstream oss;
	oss << "[File] " << mFile << std::endl << "[Line] " << mLine;
	return oss.str();
}

void ChiliException::GenerateMessage() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	mWideWhatBuffer = oss.str();
}
