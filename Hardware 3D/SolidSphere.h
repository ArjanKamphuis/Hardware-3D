#pragma once

#include "Drawable.h"

class SolidSphere : public Drawable
{
public:
	SolidSphere(const Graphics& gfx, float radius);
	void XM_CALLCONV SetPosition(DirectX::FXMVECTOR position) noexcept;
	void SetMaterial(const Graphics& gfx, const Material& material) noexcept;
	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override;

private:
	float mRadius;
	DirectX::XMFLOAT3 mPosition = {};
	Material mMaterial = {};
};

