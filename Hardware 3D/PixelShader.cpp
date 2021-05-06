#include "PixelShader.h"

#include "GraphicsThrowMacros.h"

namespace Bind
{
	PixelShader::PixelShader(const Graphics& gfx, const std::wstring& path)
	{
		INFOMAN(gfx);

		Microsoft::WRL::ComPtr<ID3DBlob> pBlob;
		GFX_THROW_INFO(D3DReadFileToBlob(path.c_str(), &pBlob));
		GFX_THROW_INFO(GetDevice(gfx)->CreatePixelShader(pBlob->GetBufferPointer(), pBlob->GetBufferSize(), nullptr, &mShader));
	}

	void PixelShader::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->PSSetShader(mShader.Get(), nullptr, 0u);
	}
}
