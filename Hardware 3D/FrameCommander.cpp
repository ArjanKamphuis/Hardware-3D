#include "FrameCommander.h"

#include "PerfLog.h"

using namespace DirectX;

FrameCommander::FrameCommander(const Graphics& gfx)
	: mDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight()), mRenderTarget1(gfx, gfx.GetWidth(), gfx.GetHeight()), mRenderTarget2(gfx, gfx.GetWidth(), gfx.GetHeight()), mBlur(gfx)
{
	Dvtx::VertexLayout layout;
	layout.Append(Dvtx::VertexLayout::ElementType::Position2D);

	Dvtx::VertexBuffer bufFull{ layout };
	bufFull.EmplaceBack(XMFLOAT2{ -1.0f, 1.0f });
	bufFull.EmplaceBack(XMFLOAT2{ 1.0f ,1.0f });
	bufFull.EmplaceBack(XMFLOAT2{ -1.0f ,-1.0f });
	bufFull.EmplaceBack(XMFLOAT2{ 1.0f ,-1.0f });

	mVBFull = Bind::VertexBuffer::Resolve(gfx, L"$Full", std::move(bufFull));
	std::vector<unsigned short> indices = { 0u, 1u, 2u, 1u, 3u, 2u };
	mIBFull = Bind::IndexBuffer::Resolve(gfx, L"$Full", std::move(indices));
	mVSFull = Bind::VertexShader::Resolve(gfx, L"Fullscreen_VS.cso");
	mLayoutFull = Bind::InputLayout::Resolve(gfx, layout, mVSFull->GetByteCode());
	mSamplerFull = Bind::Sampler::Resolve(gfx, false, true);
}

void FrameCommander::Accept(Job job, size_t target) noexcept
{
	mPasses[target].Accept(job);
}

void FrameCommander::Execute(const Graphics& gfx) noexcept(!IS_DEBUG)
{
	using namespace Bind;

	mDepthStencil.Clear(gfx);
	mRenderTarget1.Clear(gfx);
	mRenderTarget2.Clear(gfx);

	// Normal stuff
	mRenderTarget1.BindAsTarget(gfx, mDepthStencil, 0u);
	Blender::Resolve(gfx, false)->Bind(gfx);
	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	mPasses[0].Excecute(gfx);

	// Binds for both blurs
	mVBFull->Bind(gfx);
	mIBFull->Bind(gfx);
	mVSFull->Bind(gfx);
	mLayoutFull->Bind(gfx);
	mSamplerFull->Bind(gfx);
	mBlur.Bind(gfx);

	// Horizontal blur
	mRenderTarget2.BindAsTarget(gfx, 0u);
	mRenderTarget1.BindAsTexture(gfx, 0u);
	mBlur.SetHorizontal(gfx);
	gfx.DrawIndexed(mIBFull->GetCount());

	// Vertical blur
	gfx.BindSwapBuffer();
	mRenderTarget2.BindAsTexture(gfx, 0u);
	mBlur.SetVertical(gfx);
	gfx.DrawIndexed(mIBFull->GetCount());
}

void FrameCommander::Reset() noexcept
{
	for (auto& pass : mPasses)
		pass.Reset();
}
