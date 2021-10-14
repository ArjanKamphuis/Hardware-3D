#include "GraphicsResource.h"

ID3D11Device* GraphicsResource::GetDevice(const Graphics& gfx) noexcept
{
    return gfx.mDevice.Get();
}

ID3D11DeviceContext* GraphicsResource::GetDeviceContext(const Graphics& gfx) noexcept
{
    return gfx.mDeviceContext.Get();
}

DxgiInfoManager& GraphicsResource::GetInfoManager(const Graphics& gfx)
{
#if defined(DEBUG) | defined(_DEBUG)
    return gfx.mInfoManager;
#else
    throw std::logic_error("YouFuckedUp! (tried to access gfx.mInfoManager in Release config)");
#endif
}
