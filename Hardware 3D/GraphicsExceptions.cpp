#include "Graphics.h"

#include <sstream>
#include "dxerr.h"

Graphics::HrException::HrException(int line, const wchar_t* file, HRESULT hr, std::vector<std::wstring> infoMsgs) noexcept
	: Exception(line, file), mHR(hr)
{
	for (const std::wstring& m : infoMsgs)
	{
		mInfo += m;
		mInfo.push_back(L'\n');
	}

	if (!mInfo.empty())
		mInfo.pop_back();
}

const wchar_t* Graphics::HrException::GetType() const noexcept
{
	return L"Chili Graphics Exception";
}

HRESULT Graphics::HrException::GetErrorCode() const noexcept
{
	return mHR;
}

std::wstring Graphics::HrException::GetErrorString() const noexcept
{
	return DXGetErrorString(mHR);
}

std::wstring Graphics::HrException::GetErrorDescription() const noexcept
{
	// delete the /r/n nasty line
	std::wstring desc = DXGetErrorDescription(mHR);
	desc.erase(std::prev(desc.end(), 2), desc.end());
	return desc;
}

std::wstring Graphics::HrException::GetErrorInfo() const noexcept
{
	return mInfo;
}

void Graphics::HrException::GenerateMessage() const noexcept
{
	std::wstringstream oss;
	oss << GetType() << std::endl
		<< L"[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() << std::dec << L" (" << static_cast<unsigned long>(GetErrorCode()) << L" )" << std::endl
		<< L"[Error String] " << GetErrorString() << std::endl
		<< L"[Description] " << GetErrorDescription() << std::endl;
	if (!mInfo.empty())
		oss << L"\r\n[Error Info]\r\n" << GetErrorInfo() << std::endl << std::endl;
	oss << GetOriginString();
	mWideWhatBuffer = oss.str();
}

Graphics::InfoException::InfoException(int line, const wchar_t* file, std::vector<std::wstring> infoMsgs) noexcept
	: Exception(line, file)
{
	for (const std::wstring& m : infoMsgs)
	{
		mInfo += m;
		mInfo.push_back(L'\n');
	}

	if (!mInfo.empty())
		mInfo.pop_back();
}

const wchar_t* Graphics::InfoException::GetType() const noexcept
{
	return L"Chili Graphics Info Exception";
}

std::wstring Graphics::InfoException::GetErrorInfo() const noexcept
{
	return mInfo;
}

void Graphics::InfoException::GenerateMessage() const noexcept
{
	std::wstringstream oss;
	oss << GetType() << std::endl << L"\r\n[Error Info]\r\n" << GetErrorInfo() << std::endl << std::endl << GetOriginString();
	mWideWhatBuffer = oss.str();
}

const wchar_t* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return L"Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}
