#include "Drawable.h"

#include <cassert>
#include <typeinfo>
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"

void Drawable::Draw(const Graphics& gfx) const noexcept(!IS_DEBUG)
{
	for (auto& b : mBinds)
		b->Bind(gfx);
	for (auto& b : GetStaticBinds())
		b->Bind(gfx);
	gfx.DrawIndexed(mIndexBuffer->GetSize());
}

Drawable::Material Drawable::GetMaterial() const noexcept
{
	return {};
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	mBinds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> buffer) noexcept(!IS_DEBUG)
{
	assert("Attempting to add index buffer a second time" && mIndexBuffer == nullptr);
	mIndexBuffer = buffer.get();
	mBinds.push_back(std::move(buffer));
}
