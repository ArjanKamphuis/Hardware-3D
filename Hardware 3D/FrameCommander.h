#pragma once

#include "BindableCommon.h"
#include "Graphics.h"
#include "Job.h"
#include "Pass.h"

class FrameCommander
{
public:
	void Accept(Job job, size_t target) noexcept;
	void Execute(const Graphics& gfx) const noexcept(!IS_DEBUG);
	void Reset() noexcept;

	std::array<Pass, 3> mPasses;
};
