#include "LayoutCodex.h"

namespace Dcb
{
    CookedLayout LayoutCodex::Resolve(RawLayout&& layout) noexcept(!IS_DEBUG)
    {
        std::wstring sig = layout.GetSignature();
        auto& map = Get_().mMap;

        const auto it = map.find(sig);
        if (it != map.end())
        {
            layout.ClearRoot();
            return { it->second };
        }
        return { map.insert({ std::move(sig), layout.DeliverRoot() }).first->second };
    }

    LayoutCodex& LayoutCodex::Get_()
    {
        static LayoutCodex codex;
        return codex;
    }
}
