#include "RenderTarget.h"

#include "DepthStencil.h"
#include "GraphicsThrowMacros.h"

using namespace Microsoft::WRL;

RenderTarget::RenderTarget(const Graphics& gfx, UINT width, UINT height)
{
	INFOMAN(gfx);

	D3D11_TEXTURE2D_DESC textureDesc = {};
	textureDesc.ArraySize = 1u;
	textureDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	textureDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
	textureDesc.Height = height;
	textureDesc.MipLevels = 1u;
	textureDesc.SampleDesc = { 1u, 0u };
	textureDesc.Width = width;
	ComPtr<ID3D11Texture2D> pTexture;
	GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&textureDesc, nullptr, &pTexture));

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
	srvDesc.Format = textureDesc.Format;
	srvDesc.Texture2D.MipLevels = 1u;
	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &mTextureView));

	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc = {};
	rtvDesc.Format = textureDesc.Format;
	rtvDesc.Texture2D = D3D11_TEX2D_RTV{};
	rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	GFX_THROW_INFO(GetDevice(gfx)->CreateRenderTargetView(pTexture.Get(), &rtvDesc, &mRenderTargetView));
}

void RenderTarget::BindAsTexture(const Graphics& gfx, UINT slot) const noexcept
{
	GetDeviceContext(gfx)->PSSetShaderResources(slot, 1u, mTextureView.GetAddressOf());
}

void RenderTarget::BindAsTarget(const Graphics& gfx) const noexcept
{
	GetDeviceContext(gfx)->OMSetRenderTargets(1u, mRenderTargetView.GetAddressOf(), nullptr);
}

void RenderTarget::BindAsTarget(const Graphics& gfx, const DepthStencil& ds) const noexcept
{
	GetDeviceContext(gfx)->OMSetRenderTargets(1u, mRenderTargetView.GetAddressOf(), ds.mDepthStencilView.Get());
}
