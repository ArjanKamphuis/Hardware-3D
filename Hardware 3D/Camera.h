#pragma once

#include <DirectXMath.h>

class Camera
{
public:
	Camera() noexcept;

	DirectX::XMMATRIX XM_CALLCONV GetMatrix() const noexcept;
	DirectX::XMVECTOR XM_CALLCONV GetPosition() const noexcept;

	void SpawnControlWindow() noexcept;
	void Reset() noexcept;

	void Rotate(float dx, float dy) noexcept;
	void XM_CALLCONV Translate(DirectX::FXMVECTOR translation) noexcept;

	void ZoomIn() noexcept;
	void ZoomOut() noexcept;

private:
	DirectX::XMFLOAT3 mPosition;
	float mPitch;
	float mYaw;

	//static constexpr float mZoomSpeed = 1.0f;
	//static constexpr float mMinRadius = 0.1f;
	static constexpr float mMaxRadius = 80.0f;
	static constexpr float mTravelSpeed = 12.0f;
	static constexpr float mRotationSpeed = 0.004f;
};

