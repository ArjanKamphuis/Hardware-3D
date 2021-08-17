#pragma once

#include "DynamicConstantBuffer.h"

namespace Dcb
{
	class LayoutCodex
	{
	public:
		static Layout Resolve(Layout& layout) noexcept(!IS_DEBUG);
	private:
		static LayoutCodex& Get_();
	private:
		std::unordered_map<std::wstring, std::shared_ptr<LayoutElement>> mMap;
	};
}
