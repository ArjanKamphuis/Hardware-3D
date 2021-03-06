#pragma once

#include "Bindable.h"

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture(const Graphics& gfx, const std::wstring& path, UINT slot = 0);
		void Bind(const Graphics& gfx) noexcept override;

		static std::shared_ptr<Texture> Resolve(const Graphics& gfx, const std::wstring& path, UINT slot = 0);
		static std::wstring GenerateUID(const std::wstring& path, UINT slot = 0);
		std::wstring GetUID() const noexcept override;

		bool HasAlpha() const noexcept;

	private:
		bool mHasAlpha = false;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureView;
		UINT mSlot;
		std::wstring mPath;
	};
}
