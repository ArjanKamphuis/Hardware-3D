#pragma once
#include "GraphicsResource.h"

class DepthStencil;

class RenderTarget : public GraphicsResource
{
public:
	RenderTarget(const Graphics& gfx, UINT width, UINT height);
	void BindAsTexture(const Graphics& gfx, UINT slot = 0u) const noexcept;
	void BindAsTarget(const Graphics& gfx) const noexcept;
	void BindAsTarget(const Graphics& gfx, const DepthStencil& ds) const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
};

