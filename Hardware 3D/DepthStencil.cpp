#include "DepthStencil.h"
#include "GraphicsThrowMacros.h"

using namespace Microsoft::WRL;

DepthStencil::DepthStencil(const Graphics& gfx, UINT width, UINT height)
{
	INFOMAN(gfx);

	D3D11_TEXTURE2D_DESC depthBufferDesc = {};
	depthBufferDesc.ArraySize = 1u;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1u;
	depthBufferDesc.SampleDesc = { 1u, 0u };
	depthBufferDesc.Width = width;
	ComPtr<ID3D11Texture2D> pDepthStencil;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&depthBufferDesc, nullptr, &pDepthStencil));

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc = {};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	GFX_THROW_INFO(GetDevice(gfx)->CreateDepthStencilView(pDepthStencil.Get(), &dsvDesc, &mDepthStencilView));
}

void DepthStencil::BindAsDepthStencil(const Graphics& gfx) const noexcept
{
	GetDeviceContext(gfx)->OMSetRenderTargets(0u, nullptr, mDepthStencilView.Get());
}

void DepthStencil::Clear(const Graphics& gfx) const noexcept
{
	GetDeviceContext(gfx)->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0u);
}
