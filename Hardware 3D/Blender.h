#pragma once

#include "Bindable.h"

namespace Bind
{
	class Blender : public Bindable
	{
	public:
		Blender(const Graphics& gfx, bool blending);
		void Bind(const Graphics& gfx) noexcept override;

		static std::shared_ptr<Blender> Resolve(const Graphics& gfx, bool blending);
		static std::wstring GenerateUID(bool blending);
		std::wstring GetUID() const noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendState;
		bool mBlending;
	};
}
