#include "ModelException.h"

#include <sstream>

ModelException::ModelException(int line, const wchar_t* file, const std::string& info) noexcept
	: ChiliException(line, file), mInfo(info.begin(), info.end())
{
}

const wchar_t* ModelException::GetType() const noexcept
{
	return L"Chili Model Exception";
}

std::wstring ModelException::GetErrorInfo() const noexcept
{
	return mInfo;
}

void ModelException::GenerateMessage() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl << GetOriginString() << std::endl << L"[Note]" << GetErrorInfo();
	mWideWhatBuffer = oss.str();
}
