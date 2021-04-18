#pragma once

#include "DrawableBase.h"
#include "AngleParameters.h"

class Box : public DrawableBase<Box>
{
public:
	Box(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist
		, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_real_distribution<float>& bdist
		, const DirectX::XMFLOAT3& color);
	
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;
	Material GetMaterial() const noexcept override;

private:
	void StaticInitialize(const Graphics& gfx) override;

private:
	float mRadius;
	AngleParameters mAngles;
	AngleParameters mDelta;
	DirectX::XMFLOAT3 mScale;
	DirectX::XMFLOAT3 mColor;
};

