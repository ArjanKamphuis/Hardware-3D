#include "BlurPack.h"

#include "ChiliMath.h"
#include "imgui/imgui.h"

BlurPack::BlurPack(const Graphics& gfx, int radius, float sigma)
	: mRadius(radius), mSigma(sigma), mPixelShader(gfx, L"Blur_PS.cso"), mKernelBuffer(gfx), mControlbuffer(gfx, 1u)
{
	SetKernel(gfx);
}

void BlurPack::Bind(const Graphics& gfx) noexcept
{
	mPixelShader.Bind(gfx);
	mKernelBuffer.Bind(gfx);
	mControlbuffer.Bind(gfx);
}

void BlurPack::ShowWindow(const Graphics& gfx)
{
	ImGui::Begin("Blur");
	bool radiusChanged = ImGui::SliderInt("Radius", &mRadius, 0, 15);
	bool sigmaChanged = ImGui::SliderFloat("Sigma", &mSigma, 0.1f, 10.0f);
	if (radiusChanged || sigmaChanged)
		SetKernel(gfx);
	ImGui::End();
}

void BlurPack::SetHorizontal(const Graphics& gfx)
{
	mControlbuffer.Update(gfx, { TRUE });
}

void BlurPack::SetVertical(const Graphics& gfx)
{
	mControlbuffer.Update(gfx, { FALSE });
}

void BlurPack::SetKernel(const Graphics& gfx) noexcept(!IS_DEBUG)
{
	assert(mRadius <= 15);
	Kernel k;
	k.NumTaps = mRadius * 2 + 1;
	float sum = 0.0f;
	for (int i = 0; i < k.NumTaps; i++)
	{
		const float x = static_cast<float>(i - mRadius);
		const float g = ChiliMath::Gauss(x, mSigma);
		sum += g;
		k.Coefficients[i].x = g;
	}
	for (int i = 0; i < k.NumTaps ; i++)
		k.Coefficients[i].x /= sum;
	mKernelBuffer.Update(gfx, k);
}
