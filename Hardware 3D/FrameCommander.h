#pragma once

#include "BindableCommon.h"
#include "DepthStencil.h"
#include "Graphics.h"
#include "Job.h"
#include "Pass.h"
#include "RenderTarget.h"

class FrameCommander
{
public:
	FrameCommander(const Graphics& gfx);
	void Accept(Job job, size_t target) noexcept;
	void Execute(const Graphics& gfx) const noexcept(!IS_DEBUG);
	void Reset() noexcept;

private:
	std::array<Pass, 3> mPasses;
	DepthStencil mDepthStencil;
	RenderTarget mRenderTarget;

	std::shared_ptr<Bind::VertexBuffer> mVBFull;
	std::shared_ptr<Bind::IndexBuffer> mIBFull;
	std::shared_ptr<Bind::VertexShader> mVSFull;
	std::shared_ptr<Bind::PixelShader> mPSFull;
	std::shared_ptr<Bind::InputLayout> mILFull;
};
