#include "Graphics.h"

#pragma comment(lib, "d3d11.lib")

using Microsoft::WRL::ComPtr;

#define GFX_EXCEPT_NOINFO(hr) Graphics::HrException(__LINE__, __FILEW__, hr)
#define GFX_THROW_NOINFO(hrcall) { HRESULT hr__ = hrcall; if(FAILED(hr__)) throw Graphics::HrException(__LINE__, __FILEW__, hr__); }

#if defined(DEBUG) | defined(_DEBUG)
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILEW__, (hr), mInfoManager.GetMessages())
#define GFX_THROW_INFO(hrcall) { mInfoManager.Set(); HRESULT hr__ = hrcall; if (FAILED(hr__)) throw GFX_EXCEPT(hr__); }
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILEW__, hr, mInfoManager.GetMessages())
#define GFX_THROW_INFO_ONLY(call) { mInfoManager.Set(); (call); auto v = mInfoManager.GetMessages(); if (!v.empty()) throw Graphics::InfoException(__LINE__, __FILEW__, v); }
#else
#define GFX_EXCEPT(hr) Graphics::HrException(__LINE__, __FILEW__, (hr))
#define GFX_THROW_INFO(hrcall) GFX_THROW_NOINFO(hrcall)
#define GFX_DEVICE_REMOVED_EXCEPT(hr) Graphics::DeviceRemovedException(__LINE__, __FILEW__, (hr))
#define GFX_THROW_INFO_ONLY(call) (call)
#endif

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = 2;
	sd.BufferDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.OutputWindow = hWnd;
	sd.SampleDesc = { 1, 0 };
	sd.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	sd.Windowed = TRUE;

	UINT createDeviceFlags = 0;
#if defined(DEBUG) | defined(_DEBUG)
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	GFX_THROW_INFO(D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, createDeviceFlags, nullptr, 0, D3D11_SDK_VERSION, &sd, &mSwapChain, &mDevice, nullptr, &mDeviceContext));

	ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(mSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
	GFX_THROW_INFO(mDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &mRenderTargetView));
}

void Graphics::EndFrame()
{
#if defined(DEBUG) | defined(_DEBUG)
	mInfoManager.Set();
#endif

	HRESULT hr;
	if (FAILED(hr = mSwapChain->Present(1u, 0u)))
	{
		if (hr == DXGI_ERROR_DEVICE_REMOVED)
			throw GFX_DEVICE_REMOVED_EXCEPT(mDevice->GetDeviceRemovedReason());
		throw GFX_EXCEPT(hr);
	}
}

void Graphics::ClearBuffer(float r, float g, float b) noexcept
{
	const float color[] = { r, g, b, 1.0f };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), color);
}

void Graphics::DrawTestTriangle()
{
	struct Vertex
	{
		float X = 0.0f;
		float Y = 0.0f;
	};

	const Vertex vertices[] =
	{
		{ 0.0f, 0.5f },
		{ 0.5f, -0.5f },
		{ -0.5f, -0.5f }
	};

	D3D11_BUFFER_DESC vbd = {};
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = sizeof(vertices);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA vInitData = {};
	vInitData.pSysMem = vertices;

	ComPtr<ID3D11Buffer> pVertexBuffer;
	GFX_THROW_INFO(mDevice->CreateBuffer(&vbd, &vInitData, &pVertexBuffer));

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	mDeviceContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	GFX_THROW_INFO_ONLY(mDeviceContext->Draw(3u, 0u));
}
