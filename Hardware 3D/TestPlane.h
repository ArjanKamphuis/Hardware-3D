#pragma once

#include "Drawable.h"

class TestPlane : public Drawable
{
private:
	struct Material
	{
		float SpecularIntensity = 0.1f;
		float SpecularPower = 20.0f;
		float Padding[2] = {};
	};

public:
	TestPlane(const Graphics& gfx, float size);
	void XM_CALLCONV SetPosition(DirectX::FXMVECTOR position) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override;

private:
	DirectX::XMFLOAT3 mPosition = {};
	float mRoll = 0.0f;
	float mPitch = 0.0f;
	float mYaw = 0.0f;
};
