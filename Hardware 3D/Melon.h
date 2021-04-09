#pragma once

#include "DrawableBase.h"
#include "AngleParameters.h"

class Melon : public DrawableBase<Melon>
{
public:
	Melon(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_int_distribution<int>& longDist, std::uniform_int_distribution<int>& latDist);

	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

private:
	void StaticInitialize(const Graphics& gfx) override;

private:
	float mRadius;
	AngleParameters mAngles;
	AngleParameters mDelta;
};

