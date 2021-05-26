#pragma once

#include "Bindable.h"
#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	template <typename C>
	class ConstantBuffer : public Bindable
	{
	public:
		ConstantBuffer(const Graphics& gfx, UINT slot = 0)
			: ConstantBuffer(gfx, nullptr, slot)
		{
		}

		ConstantBuffer(const Graphics& gfx, const C& consts, UINT slot = 0)
			: ConstantBuffer(gfx, &consts, slot)
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
		ConstantBuffer(const Graphics& gfx, const C* consts, UINT slot)
			: mSlot(slot)
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
		UINT mSlot;
	};

	template<typename C>
	class VertexConstantBuffer : public ConstantBuffer<C>
	{
		using Bindable::GetDeviceContext;
		using ConstantBuffer<C>::mBuffer;
		using ConstantBuffer<C>::mSlot;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(const Graphics& gfx) noexcept override
		{
			GetDeviceContext(gfx)->VSSetConstantBuffers(mSlot, 1u, mBuffer.GetAddressOf());
		}

		static std::shared_ptr<VertexConstantBuffer> Resolve(const Graphics& gfx, const C& consts, UINT slot = 0)
		{
			return Codex::Resolve<VertexConstantBuffer>(gfx, consts, slot);
		}
		static std::shared_ptr<VertexConstantBuffer> Resolve(const Graphics& gfx, UINT slot = 0)
		{
			return Codex::Resolve<VertexConstantBuffer>(gfx, slot);
		}
		static std::wstring GenerateUID(const C&, UINT slot)
		{
			return GenerateUID(slot);
		}
		static std::wstring GenerateUID(UINT slot = 0)
		{
			const std::string name{ typeid(VertexConstantBuffer).name() };
			return std::wstring{ name.begin(), name.end() } + L"#s" + std::to_wstring(slot);
		}
		std::wstring GetUID() const noexcept override
		{
			return GenerateUID(mSlot);
		}
	};

	template<typename C>
	class PixelConstantBuffer : public ConstantBuffer<C>
	{
		using Bindable::GetDeviceContext;
		using ConstantBuffer<C>::mBuffer;
		using ConstantBuffer<C>::mSlot;
	public:
		using ConstantBuffer<C>::ConstantBuffer;
		void Bind(const Graphics& gfx) noexcept override
		{
			GetDeviceContext(gfx)->PSSetConstantBuffers(mSlot, 1u, mBuffer.GetAddressOf());
		}

		static std::shared_ptr<PixelConstantBuffer> Resolve(const Graphics& gfx, const C& consts, UINT slot = 0)
		{
			return Codex::Resolve<PixelConstantBuffer>(gfx, consts, slot);
		}
		static std::shared_ptr<PixelConstantBuffer> Resolve(const Graphics& gfx, UINT slot = 0)
		{
			return Codex::Resolve<PixelConstantBuffer>(gfx, slot);
		}
		static std::wstring GenerateUID(const C&, UINT slot)
		{
			return GenerateUID(slot);
		}
		static std::wstring GenerateUID(UINT slot = 0)
		{
			const std::string name{ typeid(PixelConstantBuffer).name() };
			return std::wstring{ name.begin(), name.end() } + L"#" + std::to_wstring(slot);
		}
		std::wstring GetUID() const noexcept override
		{
			return GenerateUID(mSlot);
		}
	};
}
