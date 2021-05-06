#pragma once

#include "ConditionalNoexcept.h"
#include "ConstantBuffers.h"
#include "Graphics.h"
#include "SolidSphere.h"

class PointLight
{
private:
	struct PointLightCBuf
	{
		alignas(16) DirectX::XMFLOAT3 CameraPosition;
		alignas(16) DirectX::XMFLOAT3 LightPosition;
		alignas(16) DirectX::XMFLOAT3 AmbientColor;
		alignas(16) DirectX::XMFLOAT3 DiffuseColor;
		float DiffuseIntensity;
		float AttConst;
		float AttLinear;
		float AttQuad;
	};

public:
	PointLight(const Graphics& gfx, float radius = 0.5f);
	void SpawnControlWindow() noexcept;
	void Reset() noexcept;
	void Draw(const Graphics& gfx) const noxnd;
	void Bind(const Graphics& gfx) const noexcept;
	void SetCameraPosition(const DirectX::XMVECTOR& cam) noexcept;

private:
	PointLightCBuf mBufferData;
	mutable SolidSphere mMesh;
	mutable Bind::PixelConstantBuffer<PointLightCBuf> mBuffer;
};

