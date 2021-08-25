#pragma once

#include "Graphics.h"

class Drawable;

namespace Bind
{
	class Bindable
	{
	public:
		virtual ~Bindable() = default;
		virtual void InitializeParentReference(const Drawable& parent) noexcept;
		virtual void Bind(const Graphics& gfx) noexcept = 0;
		virtual std::wstring GetUID() const noexcept;

	protected:
		static ID3D11Device* GetDevice(const Graphics& gfx) noexcept;
		static ID3D11DeviceContext* GetDeviceContext(const Graphics& gfx) noexcept;
		static DxgiInfoManager& GetInfoManager(const Graphics& gfx) noexcept(IS_DEBUG);
	};
}
