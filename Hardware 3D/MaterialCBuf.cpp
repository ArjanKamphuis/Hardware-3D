#include "MaterialCBuf.h"

std::unique_ptr<PixelConstantBuffer<Drawable::Material>> MaterialCBuf::mPCBuffer;

MaterialCBuf::MaterialCBuf(const Graphics& gfx, const Drawable& parent, UINT slot)
	: mParent(parent)
{
	if (!mPCBuffer)
		mPCBuffer = std::make_unique<PixelConstantBuffer<Drawable::Material>>(gfx, slot);
}

void MaterialCBuf::Bind(const Graphics& gfx) noexcept
{
	mPCBuffer->Update(gfx, mParent.GetMaterial());
	mPCBuffer->Bind(gfx);
}
