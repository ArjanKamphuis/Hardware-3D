#include "Blender.h"

#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	Blender::Blender(const Graphics& gfx, bool blending, std::optional<float> factor)
		: mBlending(blending)
	{
		INFOMAN(gfx);

		if (factor)
		{
			mBlendFactors.emplace();
			mBlendFactors->fill(*factor);
		}

		D3D11_BLEND_DESC blendDesc = CD3D11_BLEND_DESC{ CD3D11_DEFAULT{} };
		if (blending)
		{
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			if (factor)
			{
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_BLEND_FACTOR;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_BLEND_FACTOR;
			}
			else
			{
				blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
				blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			}
		}
		GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &mBlendState));
	}

	void Blender::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->OMSetBlendState(mBlendState.Get(), mBlendFactors ? mBlendFactors->data() : nullptr, 0xFFFFFFFFu);
	}

	void Blender::SetBlendFactor(float factor) noexcept(!IS_DEBUG)
	{
		assert(mBlendFactors);
		mBlendFactors->fill(factor);
	}

	float Blender::GetBlendFactor() const noexcept(!IS_DEBUG)
	{
		assert(mBlendFactors);
		return mBlendFactors->front();
	}

	std::shared_ptr<Blender> Blender::Resolve(const Graphics& gfx, bool blending, std::optional<float> factor)
	{
		return Codex::Resolve<Blender>(gfx, blending, factor);
	}

	std::wstring Blender::GenerateUID(bool blending, std::optional<float> factor)
	{
		const std::string name(typeid(Blender).name());
		return std::wstring{ name.begin(), name.end() } + L"#" + (blending ? L"b" : L"n") + (factor ? L"#f" + std::to_wstring(*factor) : L"");
	}

	std::wstring Blender::GetUID() const noexcept
	{
		return GenerateUID(mBlending, mBlendFactors ? mBlendFactors->front() : std::optional<float>{});
	}
}
