#include "TechniqueProbe.h"

void TechniqueProbe::SetTechnique(Technique* pTech)
{
	mTechnique = pTech;
	OnSetTechnique();
}

void TechniqueProbe::SetStep(Step* pStep)
{
	mStep = pStep;
	OnSetStep();
}

void TechniqueProbe::OnSetTechnique()
{
}

void TechniqueProbe::OnSetStep()
{
}
