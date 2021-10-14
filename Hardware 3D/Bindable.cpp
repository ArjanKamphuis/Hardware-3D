#include "Bindable.h"

namespace Bind
{
    void Bindable::InitializeParentReference(const Drawable& parent) noexcept
    {
    }

    void Bindable::Accept(TechniqueProbe&)
    {
    }

    std::wstring Bindable::GetUID() const noexcept
    {
        assert(false);
        return L"";
    }
}
