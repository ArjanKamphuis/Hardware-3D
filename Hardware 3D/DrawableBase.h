#pragma once

#include "BindableCommon.h"
#include "Drawable.h"

template<class T>
class DrawableBase : public Drawable
{
protected:
	static bool IsStaticInitialized() noexcept
	{
		return !mStaticBinds.empty();
	}
	static void AddStaticBind(std::unique_ptr<Bind::Bindable> bind) noexcept(!IS_DEBUG)
	{
		assert("*Must* use AddStaticIndexBuffer to bind index buffer" && typeid(*bind) != typeid(Bind::IndexBuffer));
		mStaticBinds.push_back(std::move(bind));
	}
	void AddStaticIndexBuffer(std::unique_ptr<Bind::IndexBuffer> buffer) noexcept(!IS_DEBUG)
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
			if (const auto p = dynamic_cast<Bind::IndexBuffer*>(b.get()))
			{
				mIndexBuffer = p;
				return;
			}
		}
		assert("Failed to find index buffer in static binds" && mIndexBuffer != nullptr);
	}

private:
	const std::vector<std::unique_ptr<Bind::Bindable>>& GetStaticBinds() const noexcept override
	{
		return mStaticBinds;
	}
	virtual void StaticInitialize(const Graphics& gfx) = 0;

protected:
	template<class Model>
	void AddRequiredStaticBindings(const Graphics& gfx, const std::wstring& vsName, const std::wstring& psName, const std::vector<D3D11_INPUT_ELEMENT_DESC>& ied,
		const Model& model,	D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		AddRequiredStaticBindings(gfx, vsName, psName, ied, model.Vertices, model.Indices, type);
	}
	template<class V, class I>
	void AddRequiredStaticBindings(const Graphics& gfx, const std::wstring& vsName, const std::wstring& psName, const std::vector<D3D11_INPUT_ELEMENT_DESC>& ied,
		const V& vertices, const I& indices, D3D11_PRIMITIVE_TOPOLOGY type = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
	{
		std::unique_ptr<Bind::VertexShader> pVS = std::make_unique<Bind::VertexShader>(gfx, vsName);
		AddStaticBind(std::make_unique<Bind::InputLayout>(gfx, ied, pVS->GetByteCode()));
		AddStaticBind(std::move(pVS));

		AddStaticBind(std::make_unique<Bind::PixelShader>(gfx, psName));
		AddStaticBind(std::make_unique<Bind::Topology>(gfx, type));
		AddStaticBind(std::make_unique<Bind::VertexBuffer>(gfx, vertices));

		AddStaticIndexBuffer(std::make_unique<Bind::IndexBuffer>(gfx, indices));
	}

private:
	static std::vector<std::unique_ptr<Bind::Bindable>> mStaticBinds;
};

template<class T>
std::vector<std::unique_ptr<Bind::Bindable>> DrawableBase<T>::mStaticBinds;
