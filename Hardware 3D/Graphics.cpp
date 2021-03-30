#include "Graphics.h"

#include <d3dcompiler.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

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

	ComPtr<ID3D11VertexShader> pVertexShader;
	ComPtr<ID3DBlob> pBlob;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	GFX_THROW_INFO(mDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
	mDeviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(mDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
	mDeviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	mDeviceContext->OMSetRenderTargets(1u, mRenderTargetView.GetAddressOf(), nullptr);

	D3D11_VIEWPORT vp = { 0, 0, 800, 600, 0, 1 };
	mDeviceContext->RSSetViewports(1u, &vp);

	GFX_THROW_INFO_ONLY(mDeviceContext->Draw(static_cast<UINT>(std::size(vertices)), 0u));
}
