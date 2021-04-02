#include "Graphics.h"

#include <cmath>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;

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

void Graphics::DrawTestTriangle(float angle, float x, float y)
{
	struct Vertex
	{
		struct
		{
			float X;
			float Y;
			float Z;
		} Position;
		struct
		{
			unsigned char R;
			unsigned char G;
			unsigned char B;
			unsigned char A;
		} Color;
	};

	const Vertex vertices[] =
	{
		{ -1.0f, -1.0f, -1.0f, 255, 0, 0, 0 },
		{ 1.0f, -1.0f, -1.0f, 0, 255, 0, 0 },
		{ -1.0f, 1.0f, -1.0f, 0, 0, 255, 0 },
		{ 1.0f, 1.0f, -1.0f, 255, 255, 0, 0 },
		{ -1.0f, -1.0f, 1.0f, 255, 0, 255, 0 },
		{ 1.0f, -1.0f, 1.0f, 0, 255, 255, 0 },
		{ -1.0f, 1.0f, 1.0f, 0, 0, 0, 0 },
		{ 1.0f, 1.0f, 1.0f, 255, 255, 255, 0 }
	};

	const unsigned short indices[] =
	{
		0, 2, 1,	2, 3, 1,
		1, 3, 5,	3, 7, 5,
		2, 6, 3,	3, 6, 7,
		4, 5, 7,	4, 7, 6,
		0, 4, 2,	2, 4, 6,
		0, 1, 4,	1, 5, 4
	};

	D3D11_BUFFER_DESC vbd = {};
	vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbd.ByteWidth = sizeof(vertices);
	vbd.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA vInitData = {};
	vInitData.pSysMem = vertices;
	ComPtr<ID3D11Buffer> pVertexBuffer;
	GFX_THROW_INFO(mDevice->CreateBuffer(&vbd, &vInitData, &pVertexBuffer));

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = sizeof(indices);
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA iInitData = {};
	iInitData.pSysMem = indices;
	ComPtr<ID3D11Buffer> pIndexBuffer;
	GFX_THROW_INFO(mDevice->CreateBuffer(&ibd, &iInitData, &pIndexBuffer));

	const UINT stride = sizeof(Vertex);
	const UINT offset = 0u;
	mDeviceContext->IASetVertexBuffers(0u, 1u, pVertexBuffer.GetAddressOf(), &stride, &offset);
	mDeviceContext->IASetIndexBuffer(pIndexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);

	struct ConstantBuffer
	{
		XMMATRIX Transform;
	};

	const ConstantBuffer cb =
	{
		XMMatrixTranspose(XMMatrixRotationZ(angle) * XMMatrixRotationX(angle) * XMMatrixTranslation(x, y, 4.0f) * XMMatrixPerspectiveLH(1.0f, 0.75f, 0.5f, 10.0f))
	};

	D3D11_BUFFER_DESC cbd = {};
	cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbd.ByteWidth = sizeof(cb);
	cbd.Usage = D3D11_USAGE_DYNAMIC;
	D3D11_SUBRESOURCE_DATA cInitData = {};
	cInitData.pSysMem = &cb;
	ComPtr<ID3D11Buffer> pConstantBuffer;
	GFX_THROW_INFO(mDevice->CreateBuffer(&cbd, &cInitData, &pConstantBuffer));
	mDeviceContext->VSSetConstantBuffers(0u, 1u, pConstantBuffer.GetAddressOf());

	ComPtr<ID3DBlob> pBlob;
	ComPtr<ID3D11PixelShader> pPixelShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"PixelShader.cso", &pBlob));
	GFX_THROW_INFO(mDevice->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pPixelShader));
	mDeviceContext->PSSetShader(pPixelShader.Get(), nullptr, 0u);

	ComPtr<ID3D11VertexShader> pVertexShader;
	GFX_THROW_INFO(D3DReadFileToBlob(L"VertexShader.cso", &pBlob));
	GFX_THROW_INFO(mDevice->CreateVertexShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &pVertexShader));
	mDeviceContext->VSSetShader(pVertexShader.Get(), nullptr, 0u);

	ComPtr<ID3D11InputLayout> pInputLayout;
	const D3D11_INPUT_ELEMENT_DESC ied[] =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0u, DXGI_FORMAT_R8G8B8A8_UNORM, 0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	GFX_THROW_INFO(mDevice->CreateInputLayout(ied, static_cast<UINT>(std::size(ied)), pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pInputLayout));
	mDeviceContext->IASetInputLayout(pInputLayout.Get());

	mDeviceContext->OMSetRenderTargets(1u, mRenderTargetView.GetAddressOf(), nullptr);

	const D3D11_VIEWPORT vp = { 0, 0, 800, 600, 0, 1 };
	mDeviceContext->RSSetViewports(1u, &vp);

	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	GFX_THROW_INFO_ONLY(mDeviceContext->DrawIndexed(static_cast<UINT>(std::size(indices)), 0u, 0u));
}
