#pragma once

#include "Drawable.h"

class TestCube : public Drawable
{
public:
	TestCube(const Graphics& gfx, float size);
	void SpawnControlWindow(const Graphics& gfx, const char* name) noexcept;

	void XM_CALLCONV SetPosition(DirectX::FXMVECTOR position) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override;

private:
	DirectX::XMFLOAT3 mPosition = {};
	float mRoll = 0.0f;
	float mPitch = 0.0f;
	float mYaw = 0.0f;
};
