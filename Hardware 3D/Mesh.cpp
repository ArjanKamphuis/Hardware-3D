#include "Mesh.h"

using namespace DirectX;

Mesh::Mesh(const Graphics& gfx, const Material& material, const aiMesh& mesh, float scale) noexcept(!IS_DEBUG)
	: Drawable(gfx, material, mesh, scale)
{
}

void XM_CALLCONV Mesh::Submit(FrameCommander& frame, FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	XMStoreFloat4x4(&mTransform, accumulatedTransform);
	Drawable::Submit(frame);
}

XMMATRIX XM_CALLCONV Mesh::GetTransformMatrix() const noexcept
{
	return XMLoadFloat4x4(&mTransform);
}
