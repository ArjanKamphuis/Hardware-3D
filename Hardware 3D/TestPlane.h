#pragma once

#include "Drawable.h"

class TestPlane : public Drawable
{
private:
	struct Material
	{
		DirectX::XMFLOAT4 Color;
	};

public:
	TestPlane(const Graphics& gfx, float size, DirectX::FXMVECTOR color = DirectX::XMVectorReplicate(1.0f));
	void SpawnControlWindow(const Graphics& gfx, const std::string& name) noexcept;

	void XM_CALLCONV SetPosition(DirectX::FXMVECTOR position) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override;

private:
	DirectX::XMFLOAT3 mPosition = {};
	float mRoll = 0.0f;
	float mPitch = 0.0f;
	float mYaw = 0.0f;
	Material mMaterial;
};
