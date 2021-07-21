#pragma once

#include "Graphics.h"

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	Drawable& operator=(const Drawable&) = delete;
	virtual ~Drawable() = default;

	void Draw(const Graphics & gfx) const noexcept(!IS_DEBUG);
	virtual DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept = 0;

	template<class T>
	T* QueryBindable() noexcept
	{
		for (auto& pBind : mBinds)
			if (auto pt = dynamic_cast<T*>(pBind.get()))
				return pt;
		return nullptr;
	}

protected:
	void AddBind(std::shared_ptr<Bind::Bindable> bind) noexcept(!IS_DEBUG);

private:
	const Bind::IndexBuffer* mIndexBuffer = nullptr;
	std::vector<std::shared_ptr<Bind::Bindable>> mBinds;
};
