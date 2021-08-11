#include "Rasterizer.h"

#include "BindableCodex.h"
#include "ChiliUtil.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	Rasterizer::Rasterizer(const Graphics& gfx, bool twoSided)
		: mTwoSided(twoSided)
	{
		INFOMAN(gfx);
		D3D11_RASTERIZER_DESC rasterizerDesc = CD3D11_RASTERIZER_DESC{ CD3D11_DEFAULT{} };
		if (twoSided) rasterizerDesc.CullMode = D3D11_CULL_NONE;
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
		return ChiliUtil::ToWide(typeid(Rasterizer).name()) + L"#" + (twoSided ? L"2s" : L"1s");
	}

	std::wstring Rasterizer::GetUID() const noexcept
	{
		return GenerateUID(mTwoSided);
	}
}
