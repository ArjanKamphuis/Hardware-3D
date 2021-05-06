#include "Texture.h"

#include "GraphicsThrowMacros.h"
#include "Surface.h"

namespace Bind
{
	Texture::Texture(const Graphics& gfx, const Surface& s)
	{
		INFOMAN(gfx);

		D3D11_TEXTURE2D_DESC texDesc = {};
		texDesc.ArraySize = 1u;
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		texDesc.Height = s.GetHeight();
		texDesc.MipLevels = 1u;
		texDesc.SampleDesc = { 1u, 0u };
		texDesc.Width = s.GetWidth();
		D3D11_SUBRESOURCE_DATA texData = {};
		texData.pSysMem = s.GetBufferPtr();
		texData.SysMemPitch = s.GetWidth() * sizeof(Surface::Color);
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&texDesc, &texData, &pTexture));

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.Texture2D.MipLevels = 1u;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &mTextureView));
	}

	void Texture::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->PSSetShaderResources(0u, 1u, mTextureView.GetAddressOf());
	}
}
