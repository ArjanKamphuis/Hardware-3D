#pragma once

#include "Bindable.h"

namespace Bind
{
	class Topology : public Bindable
	{
	public:
		Topology(const Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		void Bind(const Graphics& gfx) noexcept override;

		static std::shared_ptr<Topology> Resolve(const Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		static std::wstring GenerateUID(D3D11_PRIMITIVE_TOPOLOGY type);
		std::wstring GetUID() const noexcept override;

	private:
		D3D11_PRIMITIVE_TOPOLOGY mType;
	};
}

