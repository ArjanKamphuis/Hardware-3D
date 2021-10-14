#include "BlurPack.h"

#include "ChiliMath.h"

BlurPack::BlurPack(const Graphics& gfx, int radius, float sigma)
	: mPixelShader(gfx, L"Blur_PS.cso"), mKernelBuffer(gfx), mControlbuffer(gfx, 1u)
{
	SetKernel(gfx, radius, sigma);
}

void BlurPack::Bind(const Graphics& gfx) noexcept
{
	mPixelShader.Bind(gfx);
	mKernelBuffer.Bind(gfx);
	mControlbuffer.Bind(gfx);
}

void BlurPack::SetHorizontal(const Graphics& gfx)
{
	mControlbuffer.Update(gfx, { TRUE });
}

void BlurPack::SetVertical(const Graphics& gfx)
{
	mControlbuffer.Update(gfx, { FALSE });
}

void BlurPack::SetKernel(const Graphics& gfx, int radius, float sigma) noexcept(!IS_DEBUG)
{
	assert(radius <= 7);
	Kernel k;
	k.NumTaps = radius * 2 + 1;
	float sum = 0.0f;
	for (int i = 1; i < k.NumTaps + 1; i++)
	{
		const float x = static_cast<float>(i - (radius + 1));
		const float g = ChiliMath::Gauss(x, sigma);
		sum += g;
		k.Coefficients[i].x = g;
	}
	for (int i = 1; i < k.NumTaps + 1; i++)
		k.Coefficients[i].x /= sum;
	mKernelBuffer.Update(gfx, k);
}
