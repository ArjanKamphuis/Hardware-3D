#include "Technique.h"

#include "Drawable.h"
#include "FrameCommander.h"

Technique::Technique(const std::wstring& name, bool active) noexcept
	: mName(name), mActive(active)
{
}

void Technique::InitializeParentReferences(const Drawable& parent) noexcept
{
	for (auto& step : mSteps)
		step.InitializeParentReferences(parent);
}

void Technique::Accept(TechniqueProbe& probe)
{
	probe.SetTechnique(this);
	for (Step& step : mSteps)
		step.Accept(probe);
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

bool Technique::IsActive() const noexcept
{
	return mActive;
}

void Technique::SetActiveState(bool active) noexcept
{
	mActive = active;
}

const std::wstring& Technique::GetName() const noexcept
{
	return mName;
}
