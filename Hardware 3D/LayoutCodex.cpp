#include "LayoutCodex.h"

bool LayoutCodex::Has(const std::wstring& tag)
{
    return Get_().Has_(tag);
}

Dcb::Layout LayoutCodex::Load(const std::wstring& tag)
{
    return Get_().Load_(tag);
}

void LayoutCodex::Store(const std::wstring& tag, Dcb::Layout& layout)
{
    Get_().Store_(tag, layout);
}

LayoutCodex& LayoutCodex::Get_()
{
    static LayoutCodex codex;
    return codex;
}

bool LayoutCodex::Has_(const std::wstring& tag) const
{
    return mMap.find(tag) != mMap.end();
}

Dcb::Layout LayoutCodex::Load_(const std::wstring& tag) const
{
    return { mMap.find(tag)->second };
}

void LayoutCodex::Store_(const std::wstring& tag, Dcb::Layout& layout)
{
    auto r = mMap.insert({ tag, layout.Finalize() });
    assert(r.second);
}
