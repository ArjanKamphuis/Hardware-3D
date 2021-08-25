#pragma once

#include "Drawable.h"

class TestCube : public Drawable
{
private:
	struct Material
	{
		DirectX::XMFLOAT3 SpecularColor = { 0.1f, 0.1f, 0.1f };
		float SpecularPower = 20.0f;
	};

public:
	TestCube(const Graphics& gfx, float size);
	void SpawnControlWindow(const Graphics& gfx, const char* name) noexcept;
	void DrawOutline(const Graphics& gfx) noexcept(!IS_DEBUG);

	void XM_CALLCONV SetPosition(DirectX::FXMVECTOR position) noexcept;
	void SetRotation(float roll, float pitch, float yaw) noexcept;
	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override;

private:
	DirectX::XMFLOAT3 mPosition = {};
	float mRoll = 0.0f;
	float mPitch = 0.0f;
	float mYaw = 0.0f;
	Material mMaterial;

	std::vector<std::shared_ptr<Bind::Bindable>> mOutlineEffect;
	bool mOutlining = false;
};
