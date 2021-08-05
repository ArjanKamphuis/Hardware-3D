#pragma once

#include <array>
#include <optional>
#include "Bindable.h"

namespace Bind
{
	class Blender : public Bindable
	{
	public:
		Blender(const Graphics& gfx, bool blending, std::optional<float> factor = {});
		void Bind(const Graphics& gfx) noexcept override;

		void SetBlendFactor(float factor) noexcept(!IS_DEBUG);
		float GetBlendFactor() const noexcept(!IS_DEBUG);

		static std::shared_ptr<Blender> Resolve(const Graphics& gfx, bool blending, std::optional<float> factor = {});
		static std::wstring GenerateUID(bool blending, std::optional<float> factor);
		std::wstring GetUID() const noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11BlendState> mBlendState;
		bool mBlending;
		std::optional<std::array<float, 4u>> mBlendFactors;
	};
}
