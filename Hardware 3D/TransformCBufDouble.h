#pragma once

#include "TransformCBuf.h"

namespace Bind
{
	class TransformCBufDouble : public TransformCBuf
	{
	public:
		TransformCBufDouble(const Graphics& gfx, const Drawable& parent, UINT slotV = 0u, UINT slotP = 0u);
		void Bind(const Graphics& gfx) noexcept override;

	protected:
		void UpdateBindImp(const Graphics& gfx, const Transforms& tf) noexcept;

	private:
		static std::unique_ptr<PixelConstantBuffer<Transforms>> mPCBuffer;
	};
}
