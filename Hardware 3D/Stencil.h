#pragma once

#include "Bindable.h"

namespace Bind
{
	class Stencil : public Bindable
	{
	public:
		enum class Mode { Off, Write, Mask };

	public:
		Stencil(const Graphics& gfx, Mode mode);
		void Bind(const Graphics& gfx) noexcept override;

		static std::shared_ptr<Stencil> Resolve(const Graphics& gfx, Mode mode) noexcept(!IS_DEBUG);
		static std::wstring GenerateUID(Mode mode) noexcept;
		std::wstring GetUID() const noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mDepthStencilState;
		Mode mMode;
	};
}
