#include "Step.h"

#include "Drawable.h"
#include "FrameCommander.h"

Step::Step(size_t targetPass)
	: mTargetPass(targetPass)
{
}

void Step::InitializeParentReferences(const Drawable& parent) noexcept
{
	for (auto& bind : mBindables)
		bind->InitializeParentReference(parent);
}

void Step::AddBindable(std::shared_ptr<Bind::Bindable> bind) noexcept
{
	mBindables.push_back(std::move(bind));
}

void Step::Accept(TechniqueProbe& probe)
{
	probe.SetStep(this);
	for (auto& pb : mBindables)
		pb->Accept(probe);
}

void Step::Submit(FrameCommander& frame, const Drawable& drawable) const
{
	frame.Accept(Job{ this, &drawable }, mTargetPass);
}

void Step::Bind(const Graphics& gfx) const
{
	for (const auto& bind : mBindables)
		bind->Bind(gfx);
}
