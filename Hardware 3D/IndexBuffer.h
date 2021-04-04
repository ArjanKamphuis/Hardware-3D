#pragma once

#include "Bindable.h"

class IndexBuffer : public Bindable
{
public:
	IndexBuffer(const Graphics& gfx, const std::vector<USHORT>& indices);
	void Bind(const Graphics& gfx) noexcept override;
	UINT GetSize() const noexcept;

private:
	UINT mSize;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mBuffer;
};
