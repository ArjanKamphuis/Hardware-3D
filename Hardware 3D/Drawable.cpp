#include "Drawable.h"

#include <assimp/scene.h>
#include "BindableCommon.h"
#include "GraphicsThrowMacros.h"
#include "Material.h"

using namespace Bind;

Drawable::Drawable(const Graphics& gfx, const Material& material, const aiMesh& mesh) noexcept
{
	mVertexBuffer = material.MakeVertexBindable(gfx, mesh);
	mIndexBuffer = material.MakeIndexBindable(gfx, mesh);
	mTopology = Topology::Resolve(gfx);

	for (Technique& technique : material.GetTechniques())
		AddTechnique(std::move(technique));
}

void Drawable::AddTechnique(Technique technique) noexcept
{
	technique.InitializeParentReferences(*this);
	mTechniques.push_back(std::move(technique));
}

void Drawable::Accept(TechniqueProbe& probe)
{
	for (Technique& technique : mTechniques)
		technique.Accept(probe);
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
