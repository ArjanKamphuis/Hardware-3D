#pragma once

#include "Bindable.h"
#include "DynamicConstantBuffer.h"

namespace Bind
{
	class PixelConstantBufferEx : public Bindable
	{
	public:
		void Update(const Graphics& gfx, const Dcb::Buffer& buffer);
		void Bind(const Graphics& gfx) noexcept override;

	private:
		virtual const Dcb::LayoutElement& GetLayoutRootElement() const noexcept = 0;

	protected:
		PixelConstantBufferEx(const Graphics& gfx, const Dcb::LayoutElement& layoutRoot, UINT slot, const Dcb::Buffer* pBuffer);

	private:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;
		UINT mSlot;
	};

	class CachingPixelConstantBufferEx : public PixelConstantBufferEx
	{
	public:
		CachingPixelConstantBufferEx(const Graphics& gfx, const Dcb::CookedLayout& layout, UINT slot = 0u);
		CachingPixelConstantBufferEx(const Graphics& gfx, const Dcb::Buffer& buffer, UINT slot = 0u);

		const Dcb::LayoutElement& GetLayoutRootElement() const noexcept override;
		const Dcb::Buffer& GetBuffer() const noexcept;
		void SetBuffer(const Dcb::Buffer& buffer);
		void Bind(const Graphics& gfx) noexcept override;

	private:
		bool mDirty = false;
		Dcb::Buffer mBuffer;
	};

	class NoCachePixelConstantBufferEx : public PixelConstantBufferEx
	{
	public:
		NoCachePixelConstantBufferEx(const Graphics& gfx, const Dcb::CookedLayout& layout, UINT slot = 0u);
		NoCachePixelConstantBufferEx(const Graphics& gfx, const Dcb::Buffer& buffer, UINT slot = 0u);

		const Dcb::LayoutElement& GetLayoutRootElement() const noexcept override;

	private:
		std::shared_ptr<Dcb::LayoutElement> mLayoutRoot;
	};
}
