#pragma once

#include "DrawableBase.h"

class Box : public DrawableBase<Box>
{
private:
	struct AngularParameters
	{
		void Update(const AngularParameters& delta, float dt);

		float Roll;
		float Pitch;
		float Yaw;
		float Theta;
		float Phi;
		float Chi;
	};

public:
	Box(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist);
	
	void Update(float dt) noexcept override;
	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;

private:
	void StaticInitialize(const Graphics& gfx) override;

private:
	float mRadius;
	AngularParameters mAngles;
	AngularParameters mDelta;
};

