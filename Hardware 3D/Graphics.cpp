#include "Graphics.h"

#include <cmath>
#include <d3dcompiler.h>
#include <DirectXMath.h>
#include <memory>
#include "GraphicsThrowMacros.h"

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;
using namespace DirectX;

Graphics::Graphics(HWND hWnd)
{
	DXGI_SWAP_CHAIN_DESC sd = {};
	sd.BufferCount = mBufferCount;
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

	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
	depthStencilDesc.DepthEnable = TRUE;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ComPtr<ID3D11DepthStencilState> pDSState;
	GFX_THROW_INFO(mDevice->CreateDepthStencilState(&depthStencilDesc, &pDSState));
	mDeviceContext->OMSetDepthStencilState(pDSState.Get(), 1u);
}

Graphics::~Graphics() noexcept
{
	mSwapChain->SetFullscreenState(FALSE, nullptr);
}

void Graphics::BeginFrame(float r, float g, float b)
{
	ComPtr<ID3D11Resource> pBackBuffer;
	GFX_THROW_INFO(mSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer)));
	GFX_THROW_INFO(mDevice->CreateRenderTargetView(pBackBuffer.Get(), nullptr, &mRenderTargetView));
	mDeviceContext->OMSetRenderTargets(1u, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());

	const float color[] = { r, g, b, 1.0f };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView.Get(), color);
	mDeviceContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
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

void Graphics::DrawIndexed(UINT count) const noexcept(!IS_DEBUG)
{
	GFX_THROW_INFO_ONLY(mDeviceContext->DrawIndexed(count, 0u, 0u));
}

void Graphics::SetProjection(DirectX::FXMMATRIX proj) noexcept
{
	mProjection = proj;
}

DirectX::XMMATRIX Graphics::GetProjection() const noexcept
{
	return mProjection;
}

void Graphics::OnResize(UINT width, UINT height)
{
	if (mRenderTargetView) mRenderTargetView.Reset();
	if (mDepthStencilView) mDepthStencilView.Reset();

	GFX_THROW_INFO(mSwapChain->ResizeBuffers(mBufferCount, width, height, DXGI_FORMAT_UNKNOWN, 0));

	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.ArraySize = 1u;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Format = DXGI_FORMAT_D32_FLOAT;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1u;
	depthBufferDesc.SampleDesc = { 1, 0 };
	depthBufferDesc.Width = width;
	ComPtr<ID3D11Texture2D> pDepthStencil;
	GFX_THROW_INFO(mDevice->CreateTexture2D(&depthBufferDesc, nullptr, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	GFX_THROW_INFO(mDevice->CreateDepthStencilView(pDepthStencil.Get(), &dsvDesc, &mDepthStencilView));

	const float fwidth = static_cast<float>(width);
	const float fheight = static_cast<float>(height);

	const D3D11_VIEWPORT vp = { 0.0f, 0.0f, fwidth, fheight, 0.0f, 1.0f };
	mDeviceContext->RSSetViewports(1u, &vp);
	
	SetProjection(XMMatrixPerspectiveLH(1.0f, fheight / fwidth, 0.5f, 40.0f));
}
