#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"

template <typename C>
class ConstantBuffer : public Bindable
{
public:
	ConstantBuffer(const Graphics& gfx)
		: ConstantBuffer(gfx, nullptr)
	{
	}

	ConstantBuffer(const Graphics& gfx, const C& consts)
		: ConstantBuffer(gfx, &consts)
	{
	}

	void Update(const Graphics& gfx, const C& consts)
	{
		INFOMAN(gfx);

		D3D11_MAPPED_SUBRESOURCE msr;
		GFX_THROW_INFO(GetDeviceContext(gfx)->Map(mBuffer.Get(), 0u, D3D11_MAP_WRITE_DISCARD, 0u, &msr));
		memcpy(msr.pData, &consts, sizeof(consts));
		GetDeviceContext(gfx)->Unmap(mBuffer.Get(), 0u);
	}

private:
	ConstantBuffer(const Graphics& gfx, const C* consts)
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC cbd = {};
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.ByteWidth = sizeof(C);
		cbd.Usage = D3D11_USAGE_DYNAMIC;

		if (consts == nullptr)
		{
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, nullptr, &mBuffer));
		}
		else
		{
			D3D11_SUBRESOURCE_DATA initData = {};
			initData.pSysMem = consts;
			GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&cbd, &initData, &mBuffer));
		}
	}

protected:
	Microsoft::WRL::ComPtr<ID3D11Buffer> mBuffer;
};

template<typename C>
class VertexConstantBuffer : public ConstantBuffer<C>
{
	using Bindable::GetDeviceContext;
	using ConstantBuffer<C>::mBuffer;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(const Graphics& gfx) noexcept override
	{
		GetDeviceContext(gfx)->VSSetConstantBuffers(0u, 1u, mBuffer.GetAddressOf());
	}
};

template<typename C>
class PixelConstantBuffer : public ConstantBuffer<C>
{
	using Bindable::GetDeviceContext;
	using ConstantBuffer<C>::mBuffer;
public:
	using ConstantBuffer<C>::ConstantBuffer;
	void Bind(const Graphics& gfx) noexcept override
	{
		GetDeviceContext(gfx)->PSSetConstantBuffers(0u, 1u, mBuffer.GetAddressOf());
	}
};
