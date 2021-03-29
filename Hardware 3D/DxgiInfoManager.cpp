#include "DxgiInfoManager.h"

#include "Graphics.h"
#include "Window.h"

#pragma comment(lib, "dxguid.lib")

#define GFX_THROW_NOINFO(hrcall) { HRESULT hr__ = hrcall; if(FAILED(hr__)) throw Graphics::HrException(__LINE__, __FILEW__, hr__); }

DxgiInfoManager::DxgiInfoManager()
{
	typedef HRESULT (WINAPI* DXGIGetDebugInterface)(REFIID, void**);

	const HMODULE hModDxgiDebug = LoadLibraryEx(L"dxgidebug.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
	if (hModDxgiDebug == nullptr)
		throw CHWND_LAST_EXCEPT();

	const auto DxgiGetDebugInterface = reinterpret_cast<DXGIGetDebugInterface>(reinterpret_cast<void*>(GetProcAddress(hModDxgiDebug, "DXGIGetDebugInterface")));
	if (DxgiGetDebugInterface == nullptr)
		throw CHWND_LAST_EXCEPT();

	GFX_THROW_NOINFO(DxgiGetDebugInterface(IID_PPV_ARGS(&mDxgiInfoQueue)));
}

void DxgiInfoManager::Set() noexcept
{
	mNext = mDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
}

std::vector<std::wstring> DxgiInfoManager::GetMessages() const
{
	std::vector<std::wstring> messages;
	const auto end = mDxgiInfoQueue->GetNumStoredMessages(DXGI_DEBUG_ALL);
	for (auto it = mNext; it < end; ++it)
	{
		SIZE_T messageLength = 0;
		GFX_THROW_NOINFO(mDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, it, nullptr, &messageLength));

		std::unique_ptr<byte[]> bytes = std::make_unique<byte[]>(messageLength);
		DXGI_INFO_QUEUE_MESSAGE* pMessage = reinterpret_cast<DXGI_INFO_QUEUE_MESSAGE*>(bytes.get());

		GFX_THROW_NOINFO(mDxgiInfoQueue->GetMessage(DXGI_DEBUG_ALL, it, pMessage, &messageLength));

		if (pMessage->Severity == DXGI_INFO_QUEUE_MESSAGE_SEVERITY::DXGI_INFO_QUEUE_MESSAGE_SEVERITY_ERROR || pMessage->Severity == DXGI_INFO_QUEUE_MESSAGE_SEVERITY::DXGI_INFO_QUEUE_MESSAGE_SEVERITY_CORRUPTION)
		{
			wchar_t s[1024];
			size_t i = 0;
			mbstowcs_s(&i, s, messageLength, pMessage->pDescription, messageLength);
			messages.push_back(s);
		}
	}
	return messages;
}
