#include "ConstantBuffersEx.h"

#include "GraphicsThrowMacros.h"
#include "TechniqueProbe.h"

namespace Bind
{
	void PixelConstantBufferEx::Update(const Graphics& gfx, const Dcb::Buffer& buffer)
	{
		assert(&buffer.GetLayoutRootElement() == &GetLayoutRootElement());

		INFOMAN(gfx);
		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO(GetDeviceContext(gfx)->Map(mConstantBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
		memcpy(msr.pData, buffer.GetData(), buffer.GetSizeInBytes());
		GetDeviceContext(gfx)->Unmap(mConstantBuffer.Get(), 0u);
	}

	void PixelConstantBufferEx::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->PSSetConstantBuffers(mSlot, 1u, mConstantBuffer.GetAddressOf());
	}

	PixelConstantBufferEx::PixelConstantBufferEx(const Graphics& gfx, const Dcb::LayoutElement& layoutRoot, UINT slot, const Dcb::Buffer* pBuffer)
		: mSlot(slot)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.ByteWidth = static_cast<UINT>(layoutRoot.GetSizeInBytes());
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.Usage = D3D11_USAGE_DYNAMIC;

		if (pBuffer != nullptr)
		{
			D3D11_SUBRESOURCE_DATA csd = {};
			csd.pSysMem = pBuffer->GetData();
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &csd, &mConstantBuffer));
		}
		else
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &mConstantBuffer));
	}

	CachingPixelConstantBufferEx::CachingPixelConstantBufferEx(const Graphics& gfx, const Dcb::CookedLayout& layout, UINT slot)
		: PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr), mBuffer(Dcb::Buffer(layout))
	{
	}

	CachingPixelConstantBufferEx::CachingPixelConstantBufferEx(const Graphics& gfx, const Dcb::Buffer& buffer, UINT slot)
		: PixelConstantBufferEx(gfx, buffer.GetLayoutRootElement(),  slot, &buffer), mBuffer(buffer)
	{
	}

	const Dcb::LayoutElement& CachingPixelConstantBufferEx::GetLayoutRootElement() const noexcept
	{
		return mBuffer.GetLayoutRootElement();
	}

	const Dcb::Buffer& CachingPixelConstantBufferEx::GetBuffer() const noexcept
	{
		return mBuffer;
	}

	void CachingPixelConstantBufferEx::SetBuffer(const Dcb::Buffer& buffer)
	{
		mBuffer.CopyFrom(buffer);
		mDirty = true;
	}

	void CachingPixelConstantBufferEx::Bind(const Graphics& gfx) noexcept
	{
		if (mDirty)
		{
			Update(gfx, mBuffer);
			mDirty = false;
		}
		PixelConstantBufferEx::Bind(gfx);
	}

	void CachingPixelConstantBufferEx::Accept(TechniqueProbe& probe)
	{
		if (probe.VisitBuffer(mBuffer))
			mDirty = true;
	}

	NoCachePixelConstantBufferEx::NoCachePixelConstantBufferEx(const Graphics& gfx, const Dcb::CookedLayout& layout, UINT slot)
		: PixelConstantBufferEx(gfx, *layout.ShareRoot(), slot, nullptr), mLayoutRoot(layout.ShareRoot())
	{
	}

	NoCachePixelConstantBufferEx::NoCachePixelConstantBufferEx(const Graphics& gfx, const Dcb::Buffer& buffer, UINT slot)
		: PixelConstantBufferEx(gfx, buffer.GetLayoutRootElement(), slot, &buffer), mLayoutRoot(buffer.ShareLayoutRoot())
	{
	}

	const Dcb::LayoutElement& NoCachePixelConstantBufferEx::GetLayoutRootElement() const noexcept
	{
		return *mLayoutRoot;
	}
}
