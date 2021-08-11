#include "Texture.h"

#include "BindableCodex.h"
#include "ChiliUtil.h"
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
		texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
		texDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
		texDesc.Height = s.GetHeight();
		texDesc.MiscFlags = D3D11_RESOURCE_MISC_GENERATE_MIPS;
		texDesc.SampleDesc = { 1u, 0u };
		texDesc.Width = s.GetWidth();
		Microsoft::WRL::ComPtr<ID3D11Texture2D> pTexture;
		GFX_THROW_INFO(GetDevice(gfx)->CreateTexture2D(&texDesc, nullptr, &pTexture));
		GetDeviceContext(gfx)->UpdateSubresource(pTexture.Get(), 0u, nullptr, s.GetBufferPtrConst(), s.GetWidth() * sizeof(Surface::Color), 0u);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = texDesc.Format;
		srvDesc.Texture2D.MipLevels = -1;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		GFX_THROW_INFO(GetDevice(gfx)->CreateShaderResourceView(pTexture.Get(), &srvDesc, &mTextureView));

		GetDeviceContext(gfx)->GenerateMips(mTextureView.Get());
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
		return ChiliUtil::ToWide(typeid(Texture).name()) + L"#" + path + L"#" + std::to_wstring(slot);
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
