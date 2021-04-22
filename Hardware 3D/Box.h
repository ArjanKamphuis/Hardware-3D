#pragma once

#include "TestObject.h"

class Box : public TestObject<Box>
{
public:
	Box(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist
		, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_real_distribution<float>& bdist, const DirectX::XMFLOAT3& color);
	
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

	bool SpawnControlWindow(const Graphics& gfx, int id) noexcept;

private:
	void StaticInitialize(const Graphics& gfx) override;
	void SyncMaterial(const Graphics& gfx) noexcept(!IS_DEBUG);

private:
	DirectX::XMFLOAT3 mScale;
	Material mMaterial;
};

