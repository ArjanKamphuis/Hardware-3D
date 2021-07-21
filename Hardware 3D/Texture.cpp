#include "Texture.h"

#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"
#include "Surface.h"

namespace Bind
{
	Texture::Texture(const Graphics& gfx, const std::wstring& path, UINT slot)
		: mSlot(slot), mPath(path)
	{
		INFOMAN(gfx);
		const Surface s = Surface::FromFile(path);
		mHasAlpha = s.AlphaLoaded();

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
		GetDeviceContext(gfx)->PSSetShaderResources(mSlot, 1u, mTextureView.GetAddressOf());
	}

	std::shared_ptr<Texture> Texture::Resolve(const Graphics& gfx, const std::wstring& path, UINT slot)
	{
		return Codex::Resolve<Texture>(gfx, path, slot);
	}
	std::wstring Texture::GenerateUID(const std::wstring& path, UINT slot)
	{
		const std::string name{ typeid(Texture).name() };
		return std::wstring{ name.begin(), name.end() } + L"#" + path + L"#" + std::to_wstring(slot);
	}
	std::wstring Texture::GetUID() const noexcept
	{
		return GenerateUID(mPath, mSlot);
	}
	bool Texture::HasAlpha() const noexcept
	{
		return mHasAlpha;
	}
}
