#include "InputLayout.h"

#include "BindableCodex.h"
#include "ChiliUtil.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	InputLayout::InputLayout(const Graphics& gfx, Dvtx::VertexLayout layout, ID3DBlob* pVertexShaderByteCode)
		: mLayout(std::move(layout))
	{
		INFOMAN(gfx);

		const auto d3dlayout = mLayout.GetD3DLayout();
		GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(d3dlayout.data(), static_cast<UINT>(d3dlayout.size()),
			pVertexShaderByteCode->GetBufferPointer(), pVertexShaderByteCode->GetBufferSize(), &mInputLayout));
	}

	void InputLayout::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->IASetInputLayout(mInputLayout.Get());
	}

	std::shared_ptr<InputLayout> InputLayout::Resolve(const Graphics& gfx, Dvtx::VertexLayout layout, ID3DBlob* pVertexShaderByteCode)
	{
		return Codex::Resolve<InputLayout>(gfx, layout, pVertexShaderByteCode);
	}
	std::wstring InputLayout::GenerateUID(Dvtx::VertexLayout layout, ID3DBlob* pVertexShaderByteCode)
	{
		return ChiliUtil::ToWide(typeid(InputLayout).name()) + L"#" + layout.GetCode();
	}
	std::wstring InputLayout::GetUID() const noexcept
	{
		return GenerateUID(mLayout);
	}
}
