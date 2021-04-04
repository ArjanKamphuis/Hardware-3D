#pragma once

#include "Bindable.h"

class VertexShader : public Bindable
{
public:
	VertexShader(const Graphics& gfx, const std::wstring& path);
	void Bind(const Graphics& gfx) noexcept override;
	ID3DBlob* GetByteCode() const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3DBlob> mByteCode;
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mShader;
};
