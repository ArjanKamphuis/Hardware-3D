#pragma once

#include "Bindable.h"
#include "DynamicConstantBuffer.h"

namespace Bind
{
	class PixelConstantBufferEx : public Bindable
	{
	public:
		PixelConstantBufferEx(const Graphics& gfx, const Dcb::LayoutElement& layout, UINT slot = 0u);
		PixelConstantBufferEx(const Graphics& gfx, const Dcb::Buffer& buffer, UINT slot = 0u);

		void Update(const Graphics& gfx, const Dcb::Buffer& buffer);
		void Bind(const Graphics& gfx) noexcept override;

	private:
		PixelConstantBufferEx(const Graphics& gfx, const Dcb::LayoutElement& layout, UINT slot, const Dcb::Buffer* pBuffer);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;
		UINT mSlot;
	};
}
