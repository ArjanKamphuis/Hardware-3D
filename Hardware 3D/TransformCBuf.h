#pragma once

#include "ConstantBuffers.h"
#include "Drawable.h"

namespace Bind
{
	class TransformCBuf : public Bindable
	{
	private:
		struct Transforms
		{
			DirectX::XMMATRIX World;
			DirectX::XMMATRIX WVP;
		};

	public:
		TransformCBuf(const Graphics& gfx, const Drawable& parent, UINT slot = 0u);
		void Bind(const Graphics& gfx) noexcept override;

	private:
		static std::unique_ptr<VertexConstantBuffer<Transforms>> mVCBuffer;
		const Drawable& mParent;
	};
}
