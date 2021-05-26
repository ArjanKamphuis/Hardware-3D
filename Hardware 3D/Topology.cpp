#include "Topology.h"

#include "BindableCodex.h"

namespace Bind
{
	Topology::Topology(const Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
		: mType(type)
	{
	}

	void Topology::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->IASetPrimitiveTopology(mType);
	}

	std::shared_ptr<Bindable> Topology::Resolve(const Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY type)
	{
		return Codex::Resolve<Topology>(gfx, type);
	}
	std::wstring Topology::GenerateUID(D3D11_PRIMITIVE_TOPOLOGY type)
	{
		const std::string name{ typeid(Topology).name() };
		return std::wstring{ name.begin(), name.end() } + L"#" + std::to_wstring(type);
	}
	std::wstring Topology::GetUID() const noexcept
	{
		return GenerateUID(mType);
	}
}
