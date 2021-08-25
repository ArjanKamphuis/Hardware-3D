#pragma once

#include "Bindable.h"

namespace Bind
{
	class NullPixelShader : public Bindable
	{
	public:
		NullPixelShader(const Graphics& gfx);
		void Bind(const Graphics& gfx) noexcept override;
		static std::shared_ptr<NullPixelShader> Resolve(const Graphics& gfx) noexcept(!IS_DEBUG);
		static std::wstring GenerateUID() noexcept;
		std::wstring GetUID() const noexcept override;
	};
}
