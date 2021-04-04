#include "Topology.h"

Topology::Topology(const Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
	: mType(type)
{
}

void Topology::Bind(const Graphics& gfx) noexcept
{
	GetDeviceContext(gfx)->IASetPrimitiveTopology(mType);
}
