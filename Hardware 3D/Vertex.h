#pragma once

#include <DirectXMath.h>
#include <DirectXPackedVector.h>
#include <vector>

class VertexLayout
{
public:
	enum class ElementType { Position2D, Position3D, Texture2D, Normal, Float3Color, Float4Color, BGRAColor };
	
	class Element
	{
	public:
		Element(ElementType type, size_t offset)
			: mType(type), mOffset(offset)
		{}

		size_t GetOffsetAfter() const noexcept(!IS_DEBUG)
		{
			return mOffset + Size();
		}
		size_t GetOffset() const
		{
			return mOffset;
		}
		size_t Size() const noexcept(!IS_DEBUG)
		{
			return SizeOf(mType);
		}
		ElementType GetType() const noexcept
		{
			return mType;
		}

		static constexpr size_t SizeOf(ElementType type) noexcept(!IS_DEBUG)
		{
			switch (type)
			{
			case ElementType::Position2D: return sizeof(DirectX::XMFLOAT2);
			case ElementType::Position3D: return sizeof(DirectX::XMFLOAT3);
			case ElementType::Texture2D: return sizeof(DirectX::XMFLOAT2);
			case ElementType::Normal: return sizeof(DirectX::XMFLOAT3);
			case ElementType::Float3Color: return sizeof(DirectX::XMFLOAT3);
			case ElementType::Float4Color: return sizeof(DirectX::XMFLOAT4);
			case ElementType::BGRAColor: return sizeof(DirectX::PackedVector::XMCOLOR);
			default:
				assert("Invalid element type" && false);
				return 0u;
			}
		}

	private:
		ElementType mType;
		size_t mOffset;
	};

public:
	template<ElementType Type>
	const Element& Resolve() const noexcept(!IS_DEBUG)
	{
		for (const Element& e : mElements)
			if (e.GetType() == Type)
				return e;
		assert("Could not resolve element type" && false);
		return mElements.front();
	}

	const Element& ResolveByIndex(size_t i) const noexcept(!IS_DEBUG)
	{
		return mElements[i];
	}

	template<ElementType Type>
	VertexLayout& Append() noexcept(!IS_DEBUG)
	{
		mElements.emplace_back(Type, Size());
		return *this;
	}

	size_t Size() const noexcept(!IS_DEBUG)
	{
		return mElements.empty() ? 0u : mElements.back().GetOffsetAfter();
	}
	size_t GetElementCount() const noexcept
	{
		return mElements.size();
	}

private:
	std::vector<Element> mElements;
};

class Vertex
{
	friend class VertexBufferExp;

public:
	template<VertexLayout::ElementType Type>
	auto& Attr() noexcept(!IS_DEBUG)
	{
		const auto& element = mLayout.Resolve<Type>();
		auto pAttribute = mData + element.GetOffset();

		if constexpr(Type == VertexLayout::ElementType::Position2D)
			return *reinterpret_cast<DirectX::XMFLOAT2*>(pAttribute);
		else if constexpr(Type == VertexLayout::ElementType::Position3D)
			return *reinterpret_cast<DirectX::XMFLOAT3*>(pAttribute);
		else if constexpr(Type == VertexLayout::ElementType::Texture2D)
			return *reinterpret_cast<DirectX::XMFLOAT2*>(pAttribute);
		else if constexpr(Type == VertexLayout::ElementType::Normal)
			return *reinterpret_cast<DirectX::XMFLOAT3*>(pAttribute);
		else if constexpr(Type == VertexLayout::ElementType::Float3Color)
			return *reinterpret_cast<DirectX::XMFLOAT3*>(pAttribute);
		else if constexpr(Type == VertexLayout::ElementType::Float4Color)
			return *reinterpret_cast<DirectX::XMFLOAT4*>(pAttribute);
		else if constexpr(Type == VertexLayout::ElementType::BGRAColor)
			return *reinterpret_cast<DirectX::PackedVector::XMCOLOR*>(pAttribute);
		else
		{
			assert("Bad element type" && false);
			return *reinterpret_cast<char*>(pAttribute);
		}
	}

	template<typename T>
	void SetAttributeByIndex(size_t i, T&& value) noexcept(!IS_DEBUG)
	{
		const auto& element = mLayout.ResolveByIndex(i);
		auto pAttribute = mData + element.GetOffset();

		switch (element.GetType())
		{
		case VertexLayout::ElementType::Position2D:
			SetAttribute<DirectX::XMFLOAT2>(pAttribute, std::forward<T>(value));
			break;
		case VertexLayout::ElementType::Position3D:
			SetAttribute<DirectX::XMFLOAT3>(pAttribute, std::forward<T>(value));
			break;
		case VertexLayout::ElementType::Texture2D:
			SetAttribute<DirectX::XMFLOAT2>(pAttribute, std::forward<T>(value));
			break;
		case VertexLayout::ElementType::Normal:
			SetAttribute<DirectX::XMFLOAT3>(pAttribute, std::forward<T>(value));
			break;
		case VertexLayout::ElementType::Float3Color:
			SetAttribute<DirectX::XMFLOAT3>(pAttribute, std::forward<T>(value));
			break;
		case VertexLayout::ElementType::Float4Color:
			SetAttribute<DirectX::XMFLOAT4>(pAttribute, std::forward<T>(value));
			break;
		case VertexLayout::ElementType::BGRAColor:
			SetAttribute<DirectX::PackedVector::XMCOLOR>(pAttribute, std::forward<T>(value));
			break;
		default:
			assert("Bad element Type" && false);
			break;
		}
	}

protected:
	Vertex(char* pData, const VertexLayout& layout) noexcept(!IS_DEBUG)
		: mData(pData), mLayout(layout)
	{
		assert(pData != nullptr);
	}

private:
	template<typename First, typename ...Rest>
	void SetAttributeByIndex(size_t i, First&& first, Rest&&... rest) noexcept(!IS_DEBUG)
	{
		SetAttributeByIndex(i, std::forward<First>(first));
		SetAttributeByIndex(i + 1, std::forward<Rest>(rest)...);
	}

	template<typename Dest, typename Src>
	void SetAttribute(char* pAttribute, Src&& value) noexcept(!IS_DEBUG)
	{
		if constexpr (std::is_assignable<Dest, Src>::value)
			*reinterpret_cast<Dest*>(pAttribute) = value;
		else
			assert("Parameter attribute type mismatch" && false);
	}

private:
	char* mData = nullptr;
	const VertexLayout& mLayout;
};

class ConstVertex
{
public:
	ConstVertex(const Vertex& v) noexcept(!IS_DEBUG)
		: mVertex(v)
	{}

	template<VertexLayout::ElementType Type>
	const auto& Attr() const noexcept(!IS_DEBUG)
	{
		return const_cast<Vertex&>(mVertex).Attr<Type>();
	}

private:
	Vertex mVertex;
};

class VertexBufferExp
{
public:
	VertexBufferExp(VertexLayout layout) noexcept(!IS_DEBUG)
		: mLayout(std::move(layout))
	{}

	const VertexLayout& GetLayout() const noexcept
	{
		return mLayout;
	}
	size_t Size() const noexcept(!IS_DEBUG)
	{
		return mBuffer.size() / mLayout.Size();
	}

	template<typename ...Params>
	void EmplaceBack(Params&&... params) noexcept(!IS_DEBUG)
	{
		assert(sizeof...(params) == mLayout.GetElementCount() && "Param count doesn't match number of vertexd elements");
		mBuffer.resize(mBuffer.size() + mLayout.Size());
		Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
	}

	Vertex Back() noexcept(!IS_DEBUG)
	{
		assert(mBuffer.size() != 0u);
		return Vertex{ mBuffer.data() + mBuffer.size() - mLayout.Size(), mLayout };
	}
	Vertex Front() noexcept(!IS_DEBUG)
	{
		assert(mBuffer.size() != 0u);
		return Vertex{ mBuffer.data(), mLayout };
	}

	Vertex operator[](size_t i) noexcept(!IS_DEBUG)
	{
		assert(i < Size());
		return Vertex{ mBuffer.data() + mLayout.Size() * i, mLayout };
	}

	ConstVertex Back() const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBufferExp*>(this)->Back();
	}
	ConstVertex Front() const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBufferExp*>(this)->Front();
	}
	ConstVertex operator[](size_t i) const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBufferExp&>(*this)[i];
	}

private:
	std::vector<char> mBuffer;
	VertexLayout mLayout;
};
