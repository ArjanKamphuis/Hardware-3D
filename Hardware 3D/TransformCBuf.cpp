#include "TransformCBuf.h"

using namespace DirectX;

std::unique_ptr<VertexConstantBuffer<XMMATRIX>> TransformCBuf::mVCBuffer;

TransformCBuf::TransformCBuf(const Graphics& gfx, const Drawable& parent)
	: mParent(parent)
{
	if (!mVCBuffer)
		mVCBuffer = std::make_unique<VertexConstantBuffer<XMMATRIX>>(gfx);
}

void TransformCBuf::Bind(const Graphics& gfx) noexcept
{
	mVCBuffer->Update(gfx, XMMatrixTranspose(mParent.GetTransformMatrix() * gfx.GetProjection()));
	mVCBuffer->Bind(gfx);
}
