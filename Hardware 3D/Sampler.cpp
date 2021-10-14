#include "Sampler.h"

#include "BindableCodex.h"
#include "ChiliUtil.h"
#include "GraphicsThrowMacros.h"

namespace Bind
{
	Sampler::Sampler(const Graphics& gfx, bool anisoEnable, bool reflect)
		: mAnisoEnable(anisoEnable), mReflect(reflect)
	{
		INFOMAN(gfx);

		D3D11_SAMPLER_DESC samplerDesc = CD3D11_SAMPLER_DESC{ CD3D11_DEFAULT{} };
		samplerDesc.AddressU = samplerDesc.AddressV = reflect ? D3D11_TEXTURE_ADDRESS_MIRROR : D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.Filter = anisoEnable ? D3D11_FILTER_ANISOTROPIC : D3D11_FILTER_MIN_MAG_MIP_POINT;
		samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;
		GFX_THROW_INFO(GetDevice(gfx)->CreateSamplerState(&samplerDesc, &mSampler));
	}

	void Sampler::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->PSSetSamplers(0u, 1u, mSampler.GetAddressOf());
	}

	std::shared_ptr<Sampler> Sampler::Resolve(const Graphics& gfx, bool anisoEnable, bool reflect)
	{
		return Codex::Resolve<Sampler>(gfx, anisoEnable, reflect);
	}

	std::wstring Sampler::GenerateUID(bool anisoEnable, bool reflect)
	{
		using namespace std::string_literals;
		return ChiliUtil::ToWide(typeid(Sampler).name()) + L"#s" + (anisoEnable ? L"A"s : L"a"s) + (reflect ? L"R"s : L"r"s);
	}

	std::wstring Sampler::GetUID() const noexcept
	{
		return GenerateUID(mAnisoEnable, mReflect);
	}
}
