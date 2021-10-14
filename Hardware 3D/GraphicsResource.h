#pragma once
#include "Graphics.h"

class GraphicsResource
{
protected:
	static ID3D11Device* GetDevice(const Graphics& gfx) noexcept;
	static ID3D11DeviceContext* GetDeviceContext(const Graphics& gfx) noexcept;
	static DxgiInfoManager& GetInfoManager(const Graphics& gfx);
};

