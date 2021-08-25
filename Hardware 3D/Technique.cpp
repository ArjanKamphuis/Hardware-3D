#include "Technique.h"

#include "Drawable.h"
#include "FrameCommander.h"

void Technique::InitializeParentReferences(const Drawable& parent) noexcept
{
	for (auto& step : mSteps)
		step.InitializeParentReferences(parent);
}

void Technique::Submit(FrameCommander& frame, const Drawable& drawable) const noexcept
{
	if (mActive)
		for (const auto& step : mSteps)
			step.Submit(frame, drawable);
}

void Technique::AddStep(Step step) noexcept
{
	mSteps.push_back(std::move(step));
}

void Technique::Activate() noexcept
{
	mActive = true;
}

void Technique::Deactivate() noexcept
{
	mActive = false;
}
