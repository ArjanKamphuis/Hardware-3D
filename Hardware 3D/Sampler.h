#pragma once

#include "Bindable.h"

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler(const Graphics& gfx, bool anisoEnable, bool reflect);
		void Bind(const Graphics& gfx) noexcept override;

		static std::shared_ptr<Sampler> Resolve(const Graphics& gfx, bool anisoEnable = true, bool reflect = false);
		static std::wstring GenerateUID(bool anisoEnable, bool reflect);
		std::wstring GetUID() const noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSampler;
		bool mAnisoEnable;
		bool mReflect;
	};
}
