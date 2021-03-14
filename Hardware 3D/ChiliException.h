#pragma once

#include <exception>
#include <string>

class ChiliException : public std::exception
{
public:
	ChiliException(int line, const wchar_t* file) noexcept;
	const char* what() const noexcept override;
	const wchar_t* GetFullMessage() const noexcept;
	virtual const wchar_t* GetType() const noexcept;
	int GetLine() const noexcept;
	const std::wstring& GetFile() const noexcept;
	std::wstring GetOriginString() const noexcept;

protected:
	virtual void GenerateMessage() const noexcept;

private:
	int mLine;
	std::wstring mFile;

protected:
	mutable std::string mWhatBuffer;
	mutable std::wstring mWideWhatBuffer;
};

