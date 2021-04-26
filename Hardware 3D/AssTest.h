#pragma once

#include "TestObject.h"

class AssTest : public TestObject<AssTest>
{
public:
	AssTest(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_real_distribution<float>& scaledist, const DirectX::XMFLOAT3& color);

	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

private:
	void StaticInitialize(const Graphics& gfx) override;

private:
	Material mMaterial;
	float mScale;
};

