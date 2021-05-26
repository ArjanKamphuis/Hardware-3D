#pragma once

#include "Bindable.h"

namespace Bind
{
	class Sampler : public Bindable
	{
	public:
		Sampler(const Graphics& gfx);
		void Bind(const Graphics& gfx) noexcept override;

		static std::shared_ptr<Sampler> Resolve(const Graphics& gfx);
		static std::wstring GenerateUID();
		std::wstring GetUID() const noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSampler;
	};
}
