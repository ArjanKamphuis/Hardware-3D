#pragma once

#include "Drawable.h"

class SolidSphere : public Drawable
{
public:
	SolidSphere(const Graphics& gfx, float radius);
	void XM_CALLCONV SetPosition(DirectX::FXMVECTOR position) noexcept;
	void XM_CALLCONV SetColor(const Graphics& gfx, DirectX::FXMVECTOR color) noexcept;
	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override;

private:
	float mRadius;
	DirectX::XMFLOAT3 mPosition = {};

	struct Material
	{
		DirectX::XMFLOAT3 Color = { 1.0f, 1.0f, 1.0f };
		float Padding = 0.0f;
	} mMaterial = {};
};

