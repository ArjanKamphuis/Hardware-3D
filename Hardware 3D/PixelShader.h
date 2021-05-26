#pragma once

#include "Bindable.h"

namespace Bind
{
	class PixelShader : public Bindable
	{
	public:
		PixelShader(const Graphics& gfx, const std::wstring& path);
		void Bind(const Graphics& gfx) noexcept override;

		static std::shared_ptr<PixelShader> Resolve(const Graphics& gfx, const std::wstring& path);
		static std::wstring GenerateUID(const std::wstring& path);
		std::wstring GetUID() const noexcept override;

	private:
		std::wstring mPath;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mShader;
	};
}
