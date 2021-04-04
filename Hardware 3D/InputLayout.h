#pragma once

#include "Bindable.h"

class InputLayout : public Bindable
{
public:
	InputLayout(const Graphics& gfx, const std::vector<D3D11_INPUT_ELEMENT_DESC>& layout, ID3DBlob* pVertexShaderByteCode);
	void Bind(const Graphics& gfx) noexcept override;

private:
	Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;
};

