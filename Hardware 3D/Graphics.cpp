#include "Graphics.h"

#include <sstream>
#include "dxerr.h"

#pragma comment(lib, "d3d11.lib")

using Microsoft::WRL::ComPtr;

#define GFX_THROW_FAILED(hrcall) { HRESULT hr__ = hrcall; if(FAILED(hr__)) throw Graphics::HrException(__LINE__, __FILEW__, hr__); }
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILEW__, hr)

Graphics::HrException::HrException(int line, const wchar_t* file, HRESULT hr) noexcept
	: Exception(line, file), mHR(hr)
{
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

void Graphics::HrException::GenerateMessage() const noexcept
{
	std::wstringstream oss;
	oss << GetType() << std::endl 
		<< L"[Error Code] 0x" << std::hex << std::uppercase << GetErrorCode() << std::dec << L" (" << static_cast<unsigned long>(GetErrorCode()) << L" )" << std::endl
		<< L"[Error String] " << GetErrorString() << std::endl
		<< L"[Description] " << GetErrorDescription() << std::endl << GetOriginString();
	mWideWhatBuffer = oss.str();
}

const wchar_t* Graphics::DeviceRemovedException::GetType() const noexcept
{
	return L"Chili Graphics Exception [Device Removed] (DXGI_ERROR_DEVICE_REMOVED)";
}

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc = { 1, 0 };
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Windowed = TRUE;

	GFX_THROW_FAILED(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, 0, nullptr, 0, D3D11_SDK_VERSION, &sd, &mSwapChain, &mDevice, nullptr, &mDeviceContext));

	ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_FAILED(mSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
	GFX_THROW_FAILED(mDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &mRenderTargetView));
}

void Graphics::EndFrame()
{
	HRESULT hr;
	if (FAILED(hr = mSwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(mDevice->GetDeviceRemovedReason());
		GFX_THROW_FAILED(hr);
	}
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r, g, b, 1.0f };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), color);
}
