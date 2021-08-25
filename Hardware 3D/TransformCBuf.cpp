#include "TransformCBuf.h"

namespace Bind
{
	using namespace DirectX;

	std::unique_ptr<VertexConstantBuffer<TransformCBuf::Transforms>> TransformCBuf::mVCBuffer;

	TransformCBuf::TransformCBuf(const Graphics& gfx, UINT slot)
	{
		if (!mVCBuffer)
			mVCBuffer = std::make_unique<VertexConstantBuffer<Transforms>>(gfx, slot);
	}

	void TransformCBuf::InitializeParentReference(const Drawable& parent) noexcept
	{
		mParent = &parent;
	}

	void TransformCBuf::Bind(const Graphics& gfx) noexcept
	{
		UpdateBindImpl(gfx, GetTransforms(gfx));
	}

	void TransformCBuf::UpdateBindImpl(const Graphics& gfx, const Transforms& tf) noexcept
	{
		assert(mParent != nullptr);
		mVCBuffer->Update(gfx, tf);
		mVCBuffer->Bind(gfx);
	}

	TransformCBuf::Transforms TransformCBuf::GetTransforms(const Graphics& gfx) noexcept
	{
		assert(mParent != nullptr);
		const XMMATRIX model = mParent->GetTransformMatrix();
		return { XMMatrixTranspose(model), XMMatrixTranspose(model * gfx.GetCamera() * gfx.GetProjection()) };
	}
}
