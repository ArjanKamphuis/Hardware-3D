#pragma once

#include <unordered_map>
#include "Bindable.h"

namespace Bind
{
	class Codex
	{
	public:
		template<class T, typename ...Params>
		static std::shared_ptr<Bindable> Resolve(const Graphics& gfx, Params&&... p) noexcept(!IS_DEBUG)
		{
			static_assert(std::is_base_of<Bindable, T>::value, "Can only resolve classes derived from Bindable");
			return Get().Resolve_<T>(gfx, std::forward<Params>(p)...);
		}

	private:
		template<class T, typename ...Params>
		std::shared_ptr<Bindable> Resolve_(const Graphics& gfx, Params&&... p) noexcept(!IS_DEBUG)
		{
			const std::wstring key = T::GenerateUID(std::forward<Params>(p)...);
			const auto it = mBinds.find(key);
			if (it == mBinds.end())
			{
				std::shared_ptr<T> bind = std::make_shared<T>(gfx, std::forward<Params>(p)...);
				mBinds[key] = bind;
				return bind;
			}
			return it->second;
		}
		static Codex& Get()
		{
			static Codex codex;
			return codex;
		}

	private:
		std::unordered_map<std::wstring, std::shared_ptr<Bindable>> mBinds;
	};
}
