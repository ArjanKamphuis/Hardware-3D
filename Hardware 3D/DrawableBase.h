#pragma once

#include "BindableBase.h"
#include "Drawable.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	static bool IsStaticInitialized() noexcept
	{
		return !mStaticBinds.empty();
	}
	static void AddStaticBind(std::unique_ptr<Bindable> bind) noexcept(!IS_DEBUG)
	{
		assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(IndexBuffer));
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

protected:
	template<class Model>
	void AddRequiredStaticBindings(const Graphics& gfx, const std::wstring& vsName, const std::wstring& psName, const std::vector<D3D11_INPUT_ELEMENT_DESC>& ied, const Model& model
		, D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		std::unique_ptr<VertexShader> vs = std::make_unique<VertexShader>(gfx, vsName);
		AddStaticBind(std::make_unique<InputLayout>(gfx, ied, vs->GetByteCode()));
		AddStaticBind(std::move(vs));

		AddStaticBind(std::make_unique<PixelShader>(gfx, psName));
		AddStaticBind(std::make_unique<Topology>(gfx, type));
		AddStaticBind(std::make_unique<VertexBuffer>(gfx, model.Vertices));

		AddStaticIndexBuffer(std::make_unique<IndexBuffer>(gfx, model.Indices));
	}

private:
	static std::vector<std::unique_ptr<Bindable>> mStaticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bindable>> DrawableBase<T>::mStaticBinds;
