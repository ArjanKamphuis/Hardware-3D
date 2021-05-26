#include "BindableCodex.h"

namespace Bind
{
	std::shared_ptr<Bindable> Codex::Resolve(const std::wstring& key) noexcept(!IS_DEBUG)
	{
		return Get().Resolve_(key);
	}

	void Codex::Store(std::shared_ptr<Bindable> bind)
	{
		Get().Store_(bind);
	}

	std::shared_ptr<Bindable> Codex::Resolve_(const std::wstring& key) const noexcept(!IS_DEBUG)
	{
		auto it = mBinds.find(key);
		if (it == mBinds.end())
			return {};
		return it->second;
	}

	void Codex::Store_(std::shared_ptr<Bindable> bind)
	{
		mBinds[bind->GetUID()] = std::move(bind);
	}

	Codex& Codex::Get()
	{
		static Codex codex;
		return codex;
	}
}
