#pragma once

#include "DynamicConstantBuffer.h"

namespace Dcb
{
	class LayoutCodex
	{
	public:
		static CookedLayout Resolve(RawLayout&& layout) noexcept(!IS_DEBUG);
	private:
		static LayoutCodex& Get_();
	private:
		std::unordered_map<std::wstring, std::shared_ptr<LayoutElement>> mMap;
	};
}
