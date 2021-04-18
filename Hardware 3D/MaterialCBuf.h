#pragma once

#include "ConstantBuffers.h"
#include "Drawable.h"

class MaterialCBuf : public Bindable
{
public:
	MaterialCBuf(const Graphics& gfx, const Drawable& parent, UINT slot = 0u);
	void Bind(const Graphics& gfx) noexcept override;

private:
	static std::unique_ptr<PixelConstantBuffer<Drawable::Material>> mPCBuffer;
	const Drawable& mParent;
};
