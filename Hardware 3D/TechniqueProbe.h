#pragma once

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

	virtual bool VisitBuffer(Dcb::Buffer& buffer) = 0;

protected:
	virtual void OnSetTechnique();
	virtual void OnSetStep();

protected:
	Technique* mTechnique = nullptr;
	Step* mStep = nullptr;
};
