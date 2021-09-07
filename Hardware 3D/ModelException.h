#pragma once

#include "ChiliException.h"

class ModelException : public ChiliException
{
public:
	ModelException(int line, const wchar_t* file, const std::string& info) noexcept;
	const wchar_t* GetType() const noexcept override;
	std::wstring GetErrorInfo() const noexcept;

protected:
	void GenerateMessage() const noexcept override;

private:
	const std::wstring mInfo;
};
