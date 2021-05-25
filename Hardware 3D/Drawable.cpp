#include "Drawable.h"

#include <cassert>
#include <typeinfo>
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"

using namespace Bind;

void Drawable::Draw(const Graphics& gfx) const noexcept(!IS_DEBUG)
{
	for (auto& b : mBinds)
		b->Bind(gfx);
	gfx.DrawIndexed(mIndexBuffer->GetSize());
}

void Drawable::AddBind(std::shared_ptr<Bindable> bind) noexcept(!IS_DEBUG)
{
	if (typeid(*bind) == typeid(IndexBuffer))
	{
		assert("Binding multiple index buffers not allowed" && mIndexBuffer == nullptr);
		mIndexBuffer = &static_cast<IndexBuffer&>(*bind);
	}
	mBinds.push_back(std::move(bind));
}
