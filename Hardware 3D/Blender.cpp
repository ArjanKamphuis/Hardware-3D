#include "Blender.h"

#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	Blender::Blender(const Graphics& gfx, bool blending)
		: mBlending(blending)
	{
		INFOMAN(gfx);

		D3D11_BLEND_DESC blendDesc = {};
		blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		if (blending)
		{
			blendDesc.RenderTarget[0].BlendEnable = TRUE;
			blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
			blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
			blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
			blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ZERO;
		}
		GFX_THROW_INFO(GetDevice(gfx)->CreateBlendState(&blendDesc, &mBlendState));
	}

	void Blender::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->OMSetBlendState(mBlendState.Get(), nullptr, 0xFFFFFFFFu);
	}

	std::shared_ptr<Blender> Blender::Resolve(const Graphics& gfx, bool blending)
	{
		return Codex::Resolve<Blender>(gfx, blending);
	}

	std::wstring Blender::GenerateUID(bool blending)
	{
		const std::string name(typeid(Blender).name());
		return std::wstring{ name.begin(), name.end() } + L"#" + (blending ? L"b" : L"n");
	}

	std::wstring Blender::GetUID() const noexcept
	{
		return GenerateUID(mBlending);
	}
}
