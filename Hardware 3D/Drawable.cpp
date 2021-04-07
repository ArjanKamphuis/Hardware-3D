#include "Drawable.h"

#include <cassert>
#include <typeinfo>
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	if (typeid(*bind) == typeid(IndexBuffer))
		mIndexBuffer = static_cast<IndexBuffer*>(bind.get());
	mBinds.push_back(std::move(bind));
}

void Drawable::Draw(const Graphics& gfx) const noexcept(!IS_DEBUG)
{
	for (auto& b : mBinds)
		b->Bind(gfx);
	for (auto& b : GetStaticBinds())
		b->Bind(gfx);
	gfx.DrawIndexed(mIndexBuffer->GetSize());
}
