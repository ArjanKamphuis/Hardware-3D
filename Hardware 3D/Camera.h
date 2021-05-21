#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	DirectX::XMMATRIX XM_CALLCONV GetMatrix() const noexcept;
	DirectX::XMVECTOR XM_CALLCONV GetPosition() const noexcept;

	void SpawnControlWindow() noexcept;
	void Reset() noexcept;

	void ZoomIn() noexcept;
	void ZoomOut() noexcept;

private:
	float mRadius = 20.0f;
	float mTheta = 0.0f;
	float mPhi = 0.0f;
	float mPitch = 0.0f;
	float mYaw = 0.0f;
	float mRoll = 0.0f;

	static constexpr float mZoomSpeed = 1.0f;
	static constexpr float mMinRadius = 0.1f;
	static constexpr float mMaxRadius = 80.0f;
};

