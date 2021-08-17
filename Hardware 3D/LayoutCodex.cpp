#include "LayoutCodex.h"

namespace Dcb
{
    Layout LayoutCodex::Resolve(Layout& layout) noexcept(!IS_DEBUG)
    {
        layout.Finalize();
        std::wstring sig = layout.GetSignature();
        auto& map = Get_().mMap;

        const auto it = map.find(sig);
        return it != map.end() ? it->second : map.insert({ std::move(sig), layout.ShareRoot() }).first->second;
    }

    LayoutCodex& LayoutCodex::Get_()
    {
        static LayoutCodex codex;
        return codex;
    }
}
