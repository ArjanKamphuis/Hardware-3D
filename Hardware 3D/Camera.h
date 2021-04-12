#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	DirectX::XMMATRIX GetMatrix() const noexcept;
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;

private:
	float mRadius = 20.0f;
	float mTheta = 0.0f;
	float mPhi = 0.0f;
	float mPitch = 0.0f;
	float mYaw = 0.0f;
	float mRoll = 0.0f;
};

