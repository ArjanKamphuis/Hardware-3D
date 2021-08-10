#include "ScriptCommander.h"

#include <sstream>

ScriptCommander::Completion::Completion(const std::wstring& content) noexcept
	: ChiliException(69, L"@ScriptCommanderAbort"), mContent(content)
{
}

const wchar_t* ScriptCommander::Completion::GetType() const noexcept
{
	return L"Script Command Completed";
}

void ScriptCommander::Completion::GenerateMessage() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl << mContent << std::endl;
	mWideWhatBuffer = oss.str();
}

ScriptCommander::Exception::Exception(int line, const wchar_t* file, const std::wstring& script, const std::wstring& message) noexcept
	: ChiliException(69, L"@ScriptCommanderAbort"), mScript(script), mMessage(message)
{
}

const wchar_t* ScriptCommander::Exception::GetType() const noexcept
{
	return L"Script Command Error";
}

void ScriptCommander::Exception::GenerateMessage() const noexcept
{
	std::wostringstream oss;
	oss << GetType() << std::endl << L"[Script File] " << mScript << std::endl << mMessage;
	mWideWhatBuffer = oss.str();
}
