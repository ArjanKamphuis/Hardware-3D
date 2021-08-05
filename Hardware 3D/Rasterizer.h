#pragma once

#include "Bindable.h"

namespace Bind
{
	class Rasterizer : public Bindable
	{
	public:
		Rasterizer(const Graphics& gfx, bool twoSided);
		void Bind(const Graphics& gfx) noexcept override;

		static std::shared_ptr<Rasterizer> Resolve(const Graphics& gfx, bool twoSided);
		static std::wstring GenerateUID(bool twoSided);
		std::wstring GetUID() const noexcept override;

	private:
		Microsoft::WRL::ComPtr<ID3D11RasterizerState> mRasterizerState;
		bool mTwoSided;
	};
}
