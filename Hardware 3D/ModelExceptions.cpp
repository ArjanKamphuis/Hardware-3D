#include "Mesh.h"

#include <sstream>

Model::Exception::Exception(int line, const wchar_t* file, const std::string& info) noexcept
	: ChiliException(line, file), mInfo(info.begin(), info.end())
{
}

const wchar_t* Model::Exception::GetType() const noexcept
{
	return L"Chili Model Exception";
}

std::wstring Model::Exception::GetErrorInfo() const noexcept
{
	return mInfo;
}

void Model::Exception::GenerateMessage() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl << GetOriginString() << std::endl << L"[Note]" << GetErrorInfo();
	mWideWhatBuffer = oss.str();
}
