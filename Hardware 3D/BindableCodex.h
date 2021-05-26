#pragma once

#include <unordered_map>
#include "Bindable.h"

namespace Bind
{
	class Codex
	{
	public:
		static std::shared_ptr<Bindable> Resolve(const std::wstring& key) noexcept(!IS_DEBUG);
		static void Store(std::shared_ptr<Bindable> bind);

	private:
		std::shared_ptr<Bindable> Resolve_(const std::wstring& key) const noexcept(!IS_DEBUG);
		void Store_(std::shared_ptr<Bindable> bind);
		static Codex& Get();

	private:
		std::unordered_map<std::wstring, std::shared_ptr<Bindable>> mBinds;
	};
}
