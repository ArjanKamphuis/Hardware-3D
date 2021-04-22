#pragma once

#include "TestObject.h"

class Sheet : public TestObject<Sheet>
{
public:
	Sheet(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, const DirectX::XMFLOAT3& color = { 1.0f, 1.0f, 1.0f });

private:
	void StaticInitialize(const Graphics& gfx) override;

private:
	DirectX::XMFLOAT3 mColor;
	Material mMaterial;
};
