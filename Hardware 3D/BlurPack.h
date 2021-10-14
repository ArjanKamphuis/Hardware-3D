#pragma once

#include "BindableCommon.h"

class BlurPack
{
private:
	struct Kernel
	{
		int NumTaps = 0;
		float Paddding[3] = {};
		DirectX::XMFLOAT4 Coefficients[31] = {};
	};
	struct Control
	{
		BOOL Horizontal = FALSE;
		float Padding[3] = {};
	};

public:
	BlurPack(const Graphics& gfx, int radius = 7, float sigma = 2.6f);
	void Bind(const Graphics& gfx) noexcept;
	void ShowWindow(const Graphics& gfx);

	void SetHorizontal(const Graphics& gfx);
	void SetVertical(const Graphics& gfx);
	void SetKernel(const Graphics& gfx) noexcept(!IS_DEBUG);

private:
	int mRadius;
	float mSigma;
	Bind::PixelShader mPixelShader;
	Bind::PixelConstantBuffer<Kernel> mKernelBuffer;
	Bind::PixelConstantBuffer<Control> mControlbuffer;
};

