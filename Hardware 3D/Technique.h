#pragma once

#include "Step.h"

class Drawable;
class FrameCommander;

class Technique
{
public:
	void InitializeParentReferences(const Drawable& parent) noexcept;

	void Submit(FrameCommander& frame, const Drawable& drawable) const noexcept;
	void AddStep(Step step) noexcept;
	void Activate() noexcept;
	void Deactivate() noexcept;

private:
	bool mActive = true;
	std::vector<Step> mSteps;
};
