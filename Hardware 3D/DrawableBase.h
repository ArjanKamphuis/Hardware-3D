#pragma once

#include "Drawable.h"
#include "IndexBuffer.h"

template<class T>
class DrawableBase : public Drawable
{
public:
	bool IsStaticInitialized() const noexcept
	{
		return !mStaticBinds.empty();
	}
	void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		mStaticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<IndexBuffer> buffer) noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && mIndexBuffer == nullptr);
		mIndexBuffer = buffer.get();
		mStaticBinds.push_back(std::move(buffer));
	}
	void SetIndexFromStatic() noexcept(!IS_DEBUG)
	{
		assert("Attempting to add index buffer a second time" && mIndexBuffer == nullptr);
		for (const auto& b : mStaticBinds)
		{
			if (const auto p = dynamic_cast<IndexBuffer*>(b.get()))
			{
				mIndexBuffer = p;
				return;
			}
		}
		assert("Failed to find index buffer in static binds" && mIndexBuffer != nullptr);
	}

private:
	const std::vector<std::unique_ptr<Bindable>>& GetStaticBinds() const noexcept override
	{
		return mStaticBinds;
	}
	virtual void StaticInitialize(const Graphics& gfx) = 0;

private:
	static std::vector<std::unique_ptr<Bindable>> mStaticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::mStaticBinds;
