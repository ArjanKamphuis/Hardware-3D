#pragma once

#include "DrawableBase.h"

template<class T>
class TestObject : public DrawableBase<T>
{
private:
	struct AngleParameters
	{
		float Roll;
		float Pitch;
		float Yaw;
		float Theta;
		float Phi;
		float Chi;
	};

public:
	TestObject(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist
		, std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist)
		: mRadius(rdist(rng)), mAngles({ 0.0f, 0.0f, 0.0f, adist(rng), adist(rng), adist(rng) }), mDeltaAngles({ ddist(rng), ddist(rng), ddist(rng), odist(rng), odist(rng), odist(rng) })
	{
	}

	void Update(float dt) noexcept
	{
		mAngles.Roll = DirectX::XMScalarModAngle(mAngles.Roll + mDeltaAngles.Roll * dt);
		mAngles.Pitch = DirectX::XMScalarModAngle(mAngles.Pitch + mDeltaAngles.Pitch * dt);
		mAngles.Yaw = DirectX::XMScalarModAngle(mAngles.Yaw + mDeltaAngles.Yaw * dt);
		mAngles.Theta = DirectX::XMScalarModAngle(mAngles.Theta + mDeltaAngles.Theta * dt);
		mAngles.Phi = DirectX::XMScalarModAngle(mAngles.Phi + mDeltaAngles.Phi * dt);
		mAngles.Chi = DirectX::XMScalarModAngle(mAngles.Chi + mDeltaAngles.Chi * dt);
	}

	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override
	{
		return DirectX::XMMatrixRotationRollPitchYaw(mAngles.Pitch, mAngles.Yaw, mAngles.Roll) *
			DirectX::XMMatrixTranslation(mRadius, 0.0f, 0.0f) *
			DirectX::XMMatrixRotationRollPitchYaw(mAngles.Theta, mAngles.Phi, mAngles.Chi);
	}

protected:
	float mRadius;
	AngleParameters mAngles;
	AngleParameters mDeltaAngles;
};
