#pragma once

#include "Bindable.h"

namespace Bind
{
	class PixelShader : public Bindable
	{
	public:
		PixelShader(const Graphics& gfx, const std::wstring& path);
		void Bind(const Graphics& gfx) noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mShader;
	};
}
