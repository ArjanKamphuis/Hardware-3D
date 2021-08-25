#include "Drawable.h"

#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"

using namespace Bind;

void Drawable::AddTechnique(Technique technique) noexcept
{
	technique.InitializeParentReferences(*this);
	mTechniques.push_back(std::move(technique));
}

void Drawable::Submit(FrameCommander& frame) const noexcept
{
	for (const auto& technique : mTechniques)
		technique.Submit(frame, *this);
}

void Drawable::Bind(const Graphics& gfx) const noexcept
{
	mTopology->Bind(gfx);
	mIndexBuffer->Bind(gfx);
	mVertexBuffer->Bind(gfx);
}

UINT Drawable::GetIndexCount() const noexcept(!IS_DEBUG)
{
	return mIndexBuffer->GetCount();
}
