#pragma once

#include "Graphics.h"

class Bindable;
class IndexBuffer;

class Drawable
{
	template<class T>
	friend class DrawableBase;

public:
	Drawable() = default;
	Drawable(const Drawable&) = delete;
	Drawable& operator=(const Drawable&) = delete;
	virtual ~Drawable() = default;

	void Draw(const Graphics & gfx) const noexcept(!IS_DEBUG);

	virtual void Update(float dt) noexcept = 0;
	virtual DirectX::XMMATRIX GetTransformMatrix() const noexcept = 0;

protected:
	void AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG);
	void AddIndexBuffer(std::unique_ptr<IndexBuffer> buffer) noexcept(!IS_DEBUG);

private:
	virtual const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept = 0;

private:
	const IndexBuffer* mIndexBuffer = nullptr;
	std::vector<std::unique_ptr<Bindable>> mBinds;
};
