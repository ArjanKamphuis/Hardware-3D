#include "Graphics.h"

#include <cmath>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include "GraphicsThrowMacros.h"

#include "BindableBase.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;

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

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_THROW_INFO(mDevice->CreateDepthStencilState(&depthStencilDesc, &pDSState));
	mDeviceContext->OMSetDepthStencilState(pDSState.Get(), 1u);

	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.ArraySize = 1u;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.Height = 600u;
	depthBufferDesc.MipLevels = 1u;
	depthBufferDesc.SampleDesc = { 1, 0 };
	depthBufferDesc.Width = 800u;
	ComPtr<ID3D11Texture2D> pDepthStencil;
	GFX_THROW_INFO(mDevice->CreateTexture2D(&depthBufferDesc, nullptr, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	GFX_THROW_INFO(mDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsvDesc, &mDepthStencilView));
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
	mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
}

void Graphics::DrawTestTriangle(float angle, float x, float z)
{
	struct Vertex
	{
		struct
		{
			float X;
			float Y;
			float Z;
		} Position;
	};

	const Vertex vertices[] =
	{
		{ -1.0f, -1.0f, -1.0f },
		{ 1.0f, -1.0f, -1.0f },
		{ -1.0f, 1.0f, -1.0f },
		{ 1.0f, 1.0f, -1.0f },
		{ -1.0f, -1.0f, 1.0f },
		{ 1.0f, -1.0f, 1.0f },
		{ -1.0f, 1.0f, 1.0f },
		{ 1.0f, 1.0f, 1.0f }
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

	struct TransformBuffer
	{
		XMMATRIX Transform;
	};

	const TransformBuffer cb =
	{
		XMMatrixTranspose(XMMatrixRotationZ(angle) * XMMatrixRotationX(angle) * XMMatrixTranslation(x, 0.0f, z + 4.0f) * XMMatrixPerspectiveLH(1.0f, 0.75f, 0.5f, 10.0f))
	};

	struct ColorConstantBuffer
	{
		struct
		{
			float R;
			float G;
			float B;
			float A;
		} FaceColors[6];
	};

	const ColorConstantBuffer colorData =
	{
		{
			{ 1.0f, 0.0f, 1.0f},
			{ 1.0f, 0.0f, 0.0f},
			{ 0.0f, 1.0f, 0.0f},
			{ 0.0f, 0.0f, 1.0f},
			{ 1.0f, 1.0f, 0.0f},
			{ 0.0f, 1.0f, 1.0f}
		}
	};
	

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	

	const D3D11_VIEWPORT vp = { 0, 0, 800, 600, 0, 1 };
	mDeviceContext->RSSetViewports(1u, &vp);

	mDeviceContext->OMSetRenderTargets(1u, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

	std::vector<USHORT> ivec(std::begin(indices), std::end(indices));
	std::vector<Vertex> vvec(std::begin(vertices), std::end(vertices));

	std::vector<std::unique_ptr<Bindable>> binds;
	binds.emplace_back(std::make_unique<PixelShader>(*this, L"PixelShader.cso"));
	binds.emplace_back(std::make_unique<Topology>(*this, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	binds.emplace_back(std::make_unique<IndexBuffer>(*this, ivec));
	binds.emplace_back(std::make_unique<VertexBuffer>(*this, vvec));	

	std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(*this, L"VertexShader.cso");
	binds.emplace_back(std::make_unique<InputLayout>(*this, ied, vs->GetByteCode()));
	binds.emplace_back(std::move(vs));
	
	for (auto& bind : binds)
		bind->Bind(*this);

	VertexConstantBuffer<TransformBuffer> vcb(*this, cb);
	PixelConstantBuffer<ColorConstantBuffer> icb(*this, colorData);
	vcb.Bind(*this);
	icb.Bind(*this);

	GFX_THROW_INFO_ONLY(mDeviceContext->DrawIndexed(static_cast<UINT>(ivec.size()), 0u, 0u));
}
