#pragma once

#include <limits>

namespace Dcb
{
	class Buffer;
}

class Step;
class Technique;

class TechniqueProbe
{
public:
	void SetTechnique(Technique* pTech);
	void SetStep(Step* pStep);

	bool VisitBuffer(Dcb::Buffer& buffer);

protected:
	virtual void OnSetTechnique();
	virtual void OnSetStep();
	virtual bool OnVisitBuffer(Dcb::Buffer& buffer) = 0;

protected:
	Technique* mTechnique = nullptr;
	Step* mStep = nullptr;
	size_t mTechIdx = std::numeric_limits<size_t>::max();
	size_t mStepIdx = std::numeric_limits<size_t>::max();
	size_t mBufferIdx = std::numeric_limits<size_t>::max();
};
