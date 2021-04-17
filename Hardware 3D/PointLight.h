#pragma once

#include "ConstantBuffers.h"
#include "Graphics.h"
#include "SolidSphere.h"

class PointLight
{
private:
	struct PointLightCBuf
	{
		DirectX::XMFLOAT3 LightPosition;
		float AttConst;
		DirectX::XMFLOAT3 MaterialColor;
		float AttLinear;
		DirectX::XMFLOAT3 AmbientColor;
		float AttQuad;
		DirectX::XMFLOAT3 DiffuseColor;
		float DiffuseIntensity;
	};

public:
	PointLight(const Graphics& gfx, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Draw(const Graphics& gfx) const noexcept(!IS_DEBUG);
	void Bind(const Graphics& gfx) const noexcept;

private:
	PointLightCBuf mBufferData;
	mutable SolidSphere mMesh;
	mutable PixelConstantBuffer<PointLightCBuf> mBuffer;
};

