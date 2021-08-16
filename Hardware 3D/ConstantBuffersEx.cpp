#include "ConstantBuffersEx.h"

#include "GraphicsThrowMacros.h"

namespace Bind
{
	PixelConstantBufferEx::PixelConstantBufferEx(const Graphics& gfx, const Dcb::LayoutElement& layout, UINT slot)
		: PixelConstantBufferEx(gfx, layout, slot, nullptr)
	{
	}

	PixelConstantBufferEx::PixelConstantBufferEx(const Graphics& gfx, const Dcb::Buffer& buffer, UINT slot)
		: PixelConstantBufferEx(gfx, buffer.GetLayout(), slot, &buffer)
	{
	}

	void PixelConstantBufferEx::Update(const Graphics& gfx, const Dcb::Buffer& buffer)
	{
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

	PixelConstantBufferEx::PixelConstantBufferEx(const Graphics& gfx, const Dcb::LayoutElement& layout, UINT slot, const Dcb::Buffer* pBuffer)
		: mSlot(slot)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.ByteWidth = static_cast<UINT>(layout.GetSizeInBytes());
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
}
