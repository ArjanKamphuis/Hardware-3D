#include "ChiliException.h"

#include <sstream>

ChiliException::ChiliException(int line, const char* file) noexcept
	: mLine(line), mFile(file)
{
}

const char* ChiliException::what() const noexcept
{
	std::ostringstream oss;
	oss << GetType() << std::endl << GetOriginString();
	mWhatBuffer = oss.str();
	return mWhatBuffer.c_str();
}

const char* ChiliException::GetType() const noexcept
{
	return "Chili Exception";
}

int ChiliException::GetLine() const noexcept
{
	return mLine;
}

const std::string& ChiliException::GetFile() const noexcept
{
	return mFile;
}

std::string ChiliException::GetOriginString() const noexcept
{
	std::ostringstream oss;
	oss << "[File] " << mFile << std::endl << "[Line] " << mLine;
	return oss.str();
}
