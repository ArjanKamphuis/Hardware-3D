#pragma once

#include "Bindable.h"
#include "Graphics.h"

class Drawable;
class FrameCommander;

class Step
{
public:
	Step(size_t targetPass);
	void InitializeParentReferences(const Drawable& parent) noexcept;

	void AddBindable(std::shared_ptr<Bind::Bindable> bind) noexcept;
	void Submit(FrameCommander& frame, const Drawable& drawable) const;
	void Bind(const Graphics& gfx) const;

private:
	size_t mTargetPass;
	std::vector<std::shared_ptr<Bind::Bindable>> mBindables;
};
