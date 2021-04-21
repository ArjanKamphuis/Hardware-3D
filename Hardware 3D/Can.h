#pragma once

#include "TestObject.h"

class Can : public TestObject<Can>
{
public:
	Can(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_real_distribution<float>& bdist, std::uniform_int_distribution<int>& tdist);

private:
	void StaticInitialize(const Graphics& gfx) override;
};
