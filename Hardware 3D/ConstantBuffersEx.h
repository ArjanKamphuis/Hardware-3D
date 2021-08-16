#pragma once

#include "Bindable.h"
#include "DynamicConstantBuffer.h"

namespace Bind
{
	class PixelConstantBufferEx : public Bindable
	{
	public:
		PixelConstantBufferEx(const Graphics& gfx, std::shared_ptr<Dcb::LayoutElement> pLayout, UINT slot = 0u);
		PixelConstantBufferEx(const Graphics& gfx, const Dcb::Buffer& buffer, UINT slot = 0u);

		void Update(const Graphics& gfx, const Dcb::Buffer& buffer);
		void Bind(const Graphics& gfx) noexcept override;

		const Dcb::LayoutElement& GetLayout() const noexcept;

	private:
		PixelConstantBufferEx(const Graphics& gfx, std::shared_ptr<Dcb::LayoutElement> pLayout, UINT slot, const Dcb::Buffer* pBuffer);

	private:
		std::shared_ptr<Dcb::LayoutElement> mLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;
		UINT mSlot;
	};
}
