#pragma once

#include "Bindable.h"

namespace Bind
{
	class Topology : public Bindable
	{
	public:
		Topology(const Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type);
		void Bind(const Graphics& gfx) noexcept override;

	private:
		D3D11_PRIMITIVE_TOPOLOGY mType;
	};
}

