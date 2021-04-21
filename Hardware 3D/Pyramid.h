#pragma once

#include "TestObject.h"

class Pyramid : public TestObject<Pyramid>
{
public:
	Pyramid(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist,
		std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist);

private:
	void StaticInitialize(const Graphics& gfx) override;
};

