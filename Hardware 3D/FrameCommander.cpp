#include "FrameCommander.h"

#include "PerfLog.h"

using namespace DirectX;

FrameCommander::FrameCommander(const Graphics& gfx)
	: mDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight()), mRenderTarget(gfx, gfx.GetWidth(), gfx.GetHeight())
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
	mPSFull = Bind::PixelShader::Resolve(gfx, L"Blur_PS.cso");
	mLayoutFull = Bind::InputLayout::Resolve(gfx, layout, mVSFull->GetByteCode());
	mSamplerFull = Bind::Sampler::Resolve(gfx, false, true);
	mBlenderFull = Bind::Blender::Resolve(gfx, true);
}

void FrameCommander::Accept(Job job, size_t target) noexcept
{
	mPasses[target].Accept(job);
}

void FrameCommander::Execute(const Graphics& gfx) const noexcept(!IS_DEBUG)
{
	using namespace Bind;

	mDepthStencil.Clear(gfx);
	mRenderTarget.Clear(gfx);
	gfx.BindSwapBuffer(mDepthStencil);

	Blender::Resolve(gfx, false)->Bind(gfx);
	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	mPasses[0].Excecute(gfx);

	Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
	NullPixelShader::Resolve(gfx)->Bind(gfx);
	mPasses[1].Excecute(gfx);

	mRenderTarget.BindAsTarget(gfx, 0u);
	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	mPasses[2].Excecute(gfx);

	gfx.BindSwapBuffer(mDepthStencil);
	mRenderTarget.BindAsTexture(gfx, 0u);
	mVBFull->Bind(gfx);
	mIBFull->Bind(gfx);
	mVSFull->Bind(gfx);
	mPSFull->Bind(gfx);
	mLayoutFull->Bind(gfx);
	mSamplerFull->Bind(gfx);
	mBlenderFull->Bind(gfx);
	Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
	gfx.DrawIndexed(mIBFull->GetCount());
}

void FrameCommander::Reset() noexcept
{
	for (auto& pass : mPasses)
		pass.Reset();
}
