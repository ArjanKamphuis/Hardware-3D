#include "TransformCBuf.h"

TransformCBuf::TransformCBuf(const Graphics& gfx, const Drawable& parent)
	: mVCBuffer(gfx), mParent(parent)
{
}

void TransformCBuf::Bind(const Graphics& gfx) noexcept
{
	mVCBuffer.Update(gfx, DirectX::XMMatrixTranspose(mParent.GetTransformMatrix() * gfx.GetProjection()));
	mVCBuffer.Bind(gfx);
}
