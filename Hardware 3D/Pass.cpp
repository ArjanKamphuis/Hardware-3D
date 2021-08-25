#include "Pass.h"

void Pass::Accept(Job job) noexcept
{
	mJobs.push_back(job);
}

void Pass::Excecute(const Graphics& gfx) const noexcept(!IS_DEBUG)
{
	for (const Job& job : mJobs)
		job.Execute(gfx);
}

void Pass::Reset() noexcept
{
	mJobs.clear();
}
