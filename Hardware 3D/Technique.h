#pragma once

#include "Step.h"

class Drawable;
class FrameCommander;

class Technique
{
public:
	Technique() = default;
	Technique(const std::wstring& name, bool active = true) noexcept;
	void InitializeParentReferences(const Drawable& parent) noexcept;

	void Accept(TechniqueProbe& probe);
	void Submit(FrameCommander& frame, const Drawable& drawable) const noexcept;
	void AddStep(Step step) noexcept;

	bool IsActive() const noexcept;
	void SetActiveState(bool active) noexcept;
	const std::wstring& GetName() const noexcept;

private:
	bool mActive = true;
	std::vector<Step> mSteps;
	std::wstring mName = L"Nameless Tech";
};
