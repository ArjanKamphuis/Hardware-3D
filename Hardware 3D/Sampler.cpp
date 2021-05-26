#include "Sampler.h"

#include "BindableCodex.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	Sampler::Sampler(const Graphics& gfx)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = samplerDesc.AddressV = samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &mSampler));
	}

	void Sampler::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->PSSetSamplers(0u, 1u, mSampler.GetAddressOf());
	}

	std::shared_ptr<Sampler> Sampler::Resolve(const Graphics& gfx)
	{
		return Codex::Resolve<Sampler>(gfx);
	}

	std::wstring Sampler::GenerateUID()
	{
		const std::string name(typeid(Sampler).name());
		return std::wstring{ name.begin(), name.end() };
	}

	std::wstring Sampler::GetUID() const noexcept
	{
		return GenerateUID();
	}
}
