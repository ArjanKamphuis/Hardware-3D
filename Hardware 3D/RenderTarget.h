#pragma once

#include "GraphicsResource.h"

class DepthStencil;

class RenderTarget : public GraphicsResource
{
public:
	RenderTarget(const Graphics& gfx, UINT width, UINT height);

	void BindAsTexture(const Graphics& gfx, UINT slot) const noexcept;
	void BindAsTarget(const Graphics& gfx, UINT unbindTextureSlot) const noexcept;
	void BindAsTarget(const Graphics& gfx, const DepthStencil& ds, UINT unbindTextureSlot) const noexcept;

	void Clear(const Graphics& gfx) const noexcept;
	void Clear(const Graphics& gfx, const std::array<float, 4u>& color) const noexcept;

private:
	void UnBindAsTexture(const Graphics& gfx, UINT slot) const noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureView;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mRenderTargetView;
};

