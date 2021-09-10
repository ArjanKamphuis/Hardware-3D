#pragma once

#include "Drawable.h"

class Material;

class Mesh : public Drawable
{
public:
	Mesh(const Graphics& gfx, const Material& material, const aiMesh& mesh, float scale = 1.0f) noexcept(!IS_DEBUG);
	void XM_CALLCONV Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override;

private:
	mutable DirectX::XMFLOAT4X4 mTransform = {};
};
