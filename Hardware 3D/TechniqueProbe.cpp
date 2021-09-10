#include "TechniqueProbe.h"

void TechniqueProbe::SetTechnique(Technique* pTech)
{
	mTechnique = pTech;
	mTechIdx++;
	OnSetTechnique();
}

void TechniqueProbe::SetStep(Step* pStep)
{
	mStep = pStep;
	mStepIdx++;
	OnSetStep();
}

bool TechniqueProbe::VisitBuffer(Dcb::Buffer& buffer)
{
	mBufferIdx++;
	return OnVisitBuffer(buffer);
}

void TechniqueProbe::OnSetTechnique()
{
}

void TechniqueProbe::OnSetStep()
{
}

bool TechniqueProbe::OnVisitBuffer(Dcb::Buffer& buffer)
{
	return false;
}
