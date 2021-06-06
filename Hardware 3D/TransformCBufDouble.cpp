#include "TransformCBufDouble.h"

namespace Bind
{
	std::unique_ptr<PixelConstantBuffer<TransformCBuf::Transforms>> TransformCBufDouble::mPCBuffer;

	TransformCBufDouble::TransformCBufDouble(const Graphics& gfx, const Drawable& parent, UINT slotV, UINT slotP)
		: TransformCBuf(gfx, parent, slotV)
	{
		if (!mPCBuffer)
			mPCBuffer = std::make_unique<PixelConstantBuffer<Transforms>>(gfx, slotP);
	}

	void TransformCBufDouble::Bind(const Graphics& gfx) noexcept
	{
		const auto tf = GetTransforms(gfx);
		TransformCBuf::UpdateBindImpl(gfx, tf);
		UpdateBindImpl(gfx, tf);
	}

	void TransformCBufDouble::UpdateBindImpl(const Graphics& gfx, const Transforms& tf) noexcept
	{
		mPCBuffer->Update(gfx, tf);
		mPCBuffer->Bind(gfx);
	}
}
