#pragma once

#include <string>
#include <vector>
#include "ChiliException.h"

class ScriptCommander
{
public:
	class Completion : public ChiliException
	{
	public:
		Completion(const std::wstring& content) noexcept;
		const wchar_t* GetType() const noexcept override;

	protected:
		void GenerateMessage() const noexcept override;

	private:
		std::wstring mContent;
	};

	class Exception : public ChiliException
	{
	public:
		Exception(int line, const wchar_t* file, const std::wstring& script = L"", const std::wstring& message = L"") noexcept;
		const wchar_t* GetType() const noexcept override;

	protected:
		void GenerateMessage() const noexcept override;

	private:
		std::wstring mScript;
		std::wstring mMessage;
	};

public:
	ScriptCommander(const std::vector<std::wstring>& args);
};

