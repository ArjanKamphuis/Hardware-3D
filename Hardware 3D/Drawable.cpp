#include "Drawable.h"

#include <cassert>
#include <typeinfo>
#include "GraphicsThrowMacros.h"
#include "IndexBuffer.h"

using namespace Bind;

void Drawable::Draw(const Graphics& gfx) const noxnd
{
	for (auto& b : mBinds)
		b->Bind(gfx);
	for (auto& b : GetStaticBinds())
		b->Bind(gfx);
	gfx.DrawIndexed(mIndexBuffer->GetSize());
}

void Drawable::AddBind(std::unique_ptr<Bindable> bind) noxnd
{
	assert("*Must* use AddIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
	mBinds.push_back(std::move(bind));
}

void Drawable::AddIndexBuffer(std::unique_ptr<IndexBuffer> buffer) noxnd
{
	assert("Attempting to add index buffer a second time" && mIndexBuffer == nullptr);
	mIndexBuffer = buffer.get();
	mBinds.push_back(std::move(buffer));
}
