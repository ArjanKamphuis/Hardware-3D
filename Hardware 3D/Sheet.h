#pragma once

#include "AngleParameters.h"
#include "DrawableBase.h"

class Sheet : public DrawableBase<Sheet>
{
public:
	Sheet(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, const DirectX::XMFLOAT3& color);

	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;
	Material GetMaterial() const noexcept override;

private:
	void StaticInitialize(const Graphics& gfx) override;

private:
	float mRadius;
	AngleParameters mAngles;
	AngleParameters mDelta;
	DirectX::XMFLOAT3 mColor;
};
