#pragma once

#include "ConstantBuffers.h"
#include "Drawable.h"

class TransformCBuf : public Bindable
{
public:
	TransformCBuf(const Graphics& gfx, const Drawable& parent);
	void Bind(const Graphics& gfx) noexcept override;

private:
	VertexConstantBuffer<DirectX::XMMATRIX> mVCBuffer;
	const Drawable& mParent;
};
