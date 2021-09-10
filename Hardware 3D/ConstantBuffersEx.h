#pragma once

#include "Bindable.h"
#include "DynamicConstantBuffer.h"
#include "TechniqueProbe.h"

namespace Bind
{
	class ConstantBufferEx : public Bindable
	{
	public:
		void Update(const Graphics& gfx, const Dcb::Buffer& buffer);
		virtual const Dcb::LayoutElement& GetLayoutRootElement() const noexcept = 0;

	protected:
		ConstantBufferEx(const Graphics& gfx, const Dcb::LayoutElement& layoutRoot, UINT slot, const Dcb::Buffer* pBuffer);

	protected:
		Microsoft::WRL::ComPtr<ID3D11Buffer> mConstantBuffer;
		UINT mSlot;
	};

	class PixelConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;
		void Bind(const Graphics& gfx) noexcept override;
	};

	class VertexConstantBufferEx : public ConstantBufferEx
	{
	public:
		using ConstantBufferEx::ConstantBufferEx;
		void Bind(const Graphics& gfx) noexcept override;
	};

	template<class T>
	class CachingConstantBufferEx : public T
	{
	public:
		CachingConstantBufferEx(const Graphics& gfx, const Dcb::CookedLayout& layout, UINT slot = 0u)
			: T(gfx, *layout.ShareRoot(), slot, nullptr), mBuffer(Dcb::Buffer(layout))
		{}
		CachingConstantBufferEx(const Graphics& gfx, const Dcb::Buffer& buffer, UINT slot = 0u)
			: T(gfx, buffer.GetLayoutRootElement(), slot, &buffer), mBuffer(buffer)
		{}

		const Dcb::LayoutElement& GetLayoutRootElement() const noexcept override
		{
			return mBuffer.GetLayoutRootElement();
		}
		const Dcb::Buffer& GetBuffer() const noexcept
		{
			return mBuffer;
		}
		void SetBuffer(const Dcb::Buffer& buffer)
		{
			mBuffer.CopyFrom(buffer);
			mDirty = true;
		}
		void Bind(const Graphics& gfx) noexcept override
		{
			if (mDirty)
			{
				T::Update(gfx, mBuffer);
				mDirty = false;
			}
			T::Bind(gfx);
		}
		void Accept(TechniqueProbe& probe) override
		{
			if (probe.VisitBuffer(mBuffer))
				mDirty = true;
		}

	private:
		bool mDirty = false;
		Dcb::Buffer mBuffer;
	};

	using CachingPixelConstantBufferEx = CachingConstantBufferEx<PixelConstantBufferEx>;
	using CachingVertexConstantBufferEx = CachingConstantBufferEx<VertexConstantBufferEx>;

	/*class NoCachePixelConstantBufferEx : public PixelConstantBufferEx
	{
	public:
		NoCachePixelConstantBufferEx(const Graphics& gfx, const Dcb::CookedLayout& layout, UINT slot = 0u);
		NoCachePixelConstantBufferEx(const Graphics& gfx, const Dcb::Buffer& buffer, UINT slot = 0u);

		const Dcb::LayoutElement& GetLayoutRootElement() const noexcept override;

	private:
		std::shared_ptr<Dcb::LayoutElement> mLayoutRoot;
	};*/
}
