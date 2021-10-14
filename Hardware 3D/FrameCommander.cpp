#include "FrameCommander.h"

#include "PerfLog.h"

using namespace std::string_literals;

FrameCommander::FrameCommander(const Graphics& gfx)
	: mDepthStencil(gfx, gfx.GetWidth(), gfx.GetHeight())
{
}

void FrameCommander::Accept(Job job, size_t target) noexcept
{
	mPasses[target].Accept(job);
}

void FrameCommander::Execute(const Graphics& gfx) const noexcept(!IS_DEBUG)
{
	using namespace Bind;

	mDepthStencil.Clear(gfx);
	gfx.BindSwapBuffer(mDepthStencil);

	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	mPasses[0].Excecute(gfx);

	Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
	NullPixelShader::Resolve(gfx)->Bind(gfx);
	mPasses[1].Excecute(gfx);

	Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
	mPasses[2].Excecute(gfx);
}

void FrameCommander::Reset() noexcept
{
	for (auto& pass : mPasses)
		pass.Reset();
}
