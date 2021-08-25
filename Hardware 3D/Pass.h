#pragma once

#include "Graphics.h"
#include "Job.h"

class Pass
{
public:
	void Accept(Job job) noexcept;
	void Excecute(const Graphics& gfx) const noexcept(!IS_DEBUG);
	void Reset() noexcept;

private:
	std::vector<Job> mJobs;
};
