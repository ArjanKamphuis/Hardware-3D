#include "TransformCBuf.h"

using namespace DirectX;

std::unique_ptr<VertexConstantBuffer<TransformCBuf::Transforms>> TransformCBuf::mVCBuffer;

TransformCBuf::TransformCBuf(const Graphics& gfx, const Drawable& parent)
	: mParent(parent)
{
	if (!mVCBuffer)
		mVCBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(gfx);
}

void TransformCBuf::Bind(const Graphics& gfx) noexcept
{
	const XMMATRIX model = mParent.GetTransformMatrix();
	const Transforms tf = { XMMatrixTranspose(model), XMMatrixTranspose(model * gfx.GetCamera() * gfx.GetProjection()) };

	mVCBuffer->Update(gfx, tf);
	mVCBuffer->Bind(gfx);
}
