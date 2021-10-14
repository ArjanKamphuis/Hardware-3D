#pragma once
#include "GraphicsResource.h"

class DepthStencil : public GraphicsResource
{
	friend class Graphics;
	friend class RenderTarget;

public:
	DepthStencil(const Graphics& gfx, UINT width, UINT height);
	void BindAsDepthStencil(const Graphics& gfx) const noexcept;
	void Clear(const Graphics& gfx) const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDepthStencilView;
};

