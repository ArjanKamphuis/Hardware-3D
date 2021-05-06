#include "InputLayout.h"

#include "GraphicsThrowMacros.h"

namespace Bind
{
	InputLayout::InputLayout(const Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode)
	{
		INFOMAN(gfx);

		GFX_THROW_INFO(GetDevice(gfx)->CreateInputLayout(layout.data(), static_cast<UINT>(layout.size()),
			pVertexShaderByteCode->GetBufferPointer(), pVertexShaderByteCode->GetBufferSize(), &mInputLayout));
	}

	void InputLayout::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->IASetInputLayout(mInputLayout.Get());
	}
}
