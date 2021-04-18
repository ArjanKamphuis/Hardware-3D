#pragma once

#include "DrawableBase.h"

class SolidSphere : public DrawableBase<SolidSphere>
{
public:
	SolidSphere(const Graphics& gfx, float radius);

	void Update(float dt) noexcept override;
	void SetPosition(const DirectX::XMFLOAT3& position) noexcept;
	void SetMaterialColor(const DirectX::XMFLOAT3& color) noexcept;

	DirectX::XMMATRIX GetTransformMatrix() const noexcept override;
	Material GetMaterial() const noexcept override;

private:
	void StaticInitialize(const Graphics& gfx) override;

private:
	float mRadius;
	DirectX::XMFLOAT3 mPosition = {};
	Material mMaterial;
};

