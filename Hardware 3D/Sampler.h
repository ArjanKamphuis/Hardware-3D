#pragma once

#include "Bindable.h"

class Sampler : public Bindable
{
public:
	Sampler(const Graphics& gfx);
	void Bind(const Graphics& gfx) noexcept override;

private:
	Microsoft::WRL::ComPtr<ID3D11SamplerState> mSampler;
};
