#pragma once

#include "Graphics.h"

namespace Bind
{
	class Bindable
	{
	public:
		virtual ~Bindable() = default;
		virtual void Bind(const Graphics& gfx) noexcept = 0;

	protected:
		static ID3D11Device* GetDevice(const Graphics& gfx) noexcept;
		static ID3D11DeviceContext* GetDeviceContext(const Graphics& gfx) noexcept;
		static DxgiInfoManager& GetInfoManager(const Graphics& gfx) noexcept(IS_DEBUG);
	};
}
