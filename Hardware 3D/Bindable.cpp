#include "Bindable.h"

#include <stdexcept>

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
    ID3D11Device* Bindable::GetDevice(const Graphics& gfx) noexcept
    {
        return gfx.mDevice.Get();
    }

    ID3D11DeviceContext* Bindable::GetDeviceContext(const Graphics& gfx) noexcept
    {
        return gfx.mDeviceContext.Get();
    }

    DxgiInfoManager& Bindable::GetInfoManager(const Graphics& gfx) noexcept(IS_DEBUG)
    {
#if defined(DEBUG) | defined(_DEBUG)
        return gfx.mInfoManager;
#else
        throw std::logic_error("YouFuckedUp! (tried to access gfx.mInfoManager in Release config)");
#endif
    }
}
