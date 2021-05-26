#pragma once

#include "Bindable.h"

namespace Bind
{
	class Texture : public Bindable
	{
	public:
		Texture(const Graphics& gfx, const std::wstring& path, UINT slot = 0);
		void Bind(const Graphics& gfx) noexcept override;

		static std::shared_ptr<Bindable> Resolve(const Graphics& gfx, const std::wstring& path, UINT slot);
		static std::wstring GenerateUID(const std::wstring& path, UINT slot);
		std::wstring GetUID() const noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureView;
		UINT mSlot;
		std::wstring mPath;
	};
}
