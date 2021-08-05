#include "Rasterizer.h"

#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	Rasterizer::Rasterizer(const Graphics& gfx, bool twoSided)
		: mTwoSided(twoSided)
	{
		INFOMAN(gfx);
		D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		GFX_THROW_INFO(GetDevice(gfx)->CreateRasterizerState(&rasterizerDesc, &mRasterizerState));
	}

	void Rasterizer::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->RSSetState(mRasterizerState.Get());
	}

	std::shared_ptr<Rasterizer> Rasterizer::Resolve(const Graphics& gfx, bool twoSided)
	{
		return Codex::Resolve<Rasterizer>(gfx, twoSided);
	}

	std::wstring Rasterizer::GenerateUID(bool twoSided)
	{
		const std::string name(typeid(Rasterizer).name());
		return std::wstring{ name.begin(), name.end() } + L"#" + (twoSided ? L"2s" : L"1s");
	}

	std::wstring Rasterizer::GetUID() const noexcept
	{
		return GenerateUID(mTwoSided);
	}
}
