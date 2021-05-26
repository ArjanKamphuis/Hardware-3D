#include "PixelShader.h"

#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	PixelShader::PixelShader(const Graphics& gfx, const std::wstring& path)
		: mPath(path)
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
	std::shared_ptr<Bindable> PixelShader::Resolve(const Graphics& gfx, const std::wstring& path)
	{
		return Codex::Resolve<PixelShader>(gfx, path);
	}
	std::wstring PixelShader::GenerateUID(const std::wstring& path)
	{
		const std::string name{ typeid(PixelShader).name() };
		return std::wstring{ name.begin(), name.end() } + L"#" + path;
	}
	std::wstring PixelShader::GetUID() const noexcept
	{
		return GenerateUID(mPath);
	}
}
