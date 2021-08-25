#include "Job.h"

#include "Drawable.h"
#include "Step.h"

Job::Job(const Step* pStep, const Drawable* pDrawable)
	: mDrawable(pDrawable), mStep(pStep)
{
}

void Job::Execute(const Graphics& gfx) const noexcept(!IS_DEBUG)
{
	mDrawable->Bind(gfx);
	mStep->Bind(gfx);
	gfx.DrawIndexed(mDrawable->GetIndexCount());
}
