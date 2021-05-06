#pragma once

#include "Bindable.h"

class Surface;

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture(const Graphics& gfx, const Surface& s);
		void Bind(const Graphics& gfx) noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureView;
	};
}
