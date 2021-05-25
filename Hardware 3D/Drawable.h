#pragma once

#include "Graphics.h"

namespace Bind
{
	class Bindable;
	class IndexBuffer;
}

class Drawable
{
	template<class T>
	friend class DrawableBase;

public:
	struct Material
	{
		DirectX::XMFLOAT3 Color = { 1.0f, 1.0f, 1.0f };
		float SpecularIntensity = 0.8f;
		float SpecularPower = 35.0f;
		float Padding[3] = {};
	};

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	Drawable& operator=(const Drawable&) = delete;
	virtual ~Drawable() = default;

	void Draw(const Graphics & gfx) const noexcept(!IS_DEBUG);

	virtual void Update(float dt) noexcept = 0;
	virtual DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept = 0;

protected:
	template<class T>
	T* QueryBindable() noexcept
	{
		for (auto& pBind : mBinds)
			if (auto pt = dynamic_cast<T*>(pBind.get()))
				return pt;
		return nullptr;
	}
	void AddBind(std::unique_ptr<Bind::Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<Bind::IndexBuffer> buffer) noexcept(!IS_DEBUG);

private:
	virtual const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept = 0;

private:
	const Bind::IndexBuffer* mIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bind::Bindable>> mBinds;
};
