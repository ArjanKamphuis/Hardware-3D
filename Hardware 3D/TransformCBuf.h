#pragma once

#include "ConstantBuffers.h"
#include "Drawable.h"

class TransformCBuf : public Bindable
{
private:
	struct Transforms
	{
		DirectX::XMMATRIX World;
		DirectX::XMMATRIX WVP;
	};

public:
	TransformCBuf(const Graphics& gfx, const Drawable& parent);
	void Bind(const Graphics& gfx) noexcept override;

private:
	static std::unique_ptr<VertexConstantBuffer<Transforms>> mVCBuffer;
	const Drawable& mParent;
};
