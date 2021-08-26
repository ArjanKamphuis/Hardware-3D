#include "FrameCommander.h"

#include "PerfLog.h"

using namespace std::string_literals;

void FrameCommander::Accept(Job job, size_t target) noexcept
{
	mPasses[target].Accept(job);
}

void FrameCommander::Execute(const Graphics& gfx) const noexcept(!IS_DEBUG)
{
	using namespace Bind;
	Stencil::Resolve(gfx, Stencil::Mode::Off)->Bind(gfx);
	mPasses[0].Excecute(gfx);

	Stencil::Resolve(gfx, Stencil::Mode::Write)->Bind(gfx);
	NullPixelShader::Resolve(gfx)->Bind(gfx);
	mPasses[1].Excecute(gfx);

	PerfLog::Start(L"Begin"s);
	Stencil::Resolve(gfx, Stencil::Mode::Mask)->Bind(gfx);
	struct SolidColorBuffer { DirectX::XMFLOAT4 Color = { 1.0f, 0.4f, 0.4f, 1.0f }; };
	PixelConstantBuffer<SolidColorBuffer>::Resolve(gfx, SolidColorBuffer{})->Bind(gfx);
	PerfLog::Mark(L"Resolve 2x"s);
	mPasses[2].Excecute(gfx);
}

void FrameCommander::Reset() noexcept
{
	for (auto& pass : mPasses)
		pass.Reset();
}
