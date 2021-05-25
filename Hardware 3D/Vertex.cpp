#include "Vertex.h"

namespace Dvtx
{
	VertexLayout::Element::Element(ElementType type, size_t offset)
		: mType(type), mOffset(offset)
	{}

	size_t VertexLayout::Element::GetOffsetAfter() const noexcept(!IS_DEBUG)
	{
		return mOffset + Size();
	}

	size_t VertexLayout::Element::GetOffset() const
	{
		return mOffset;
	}

	size_t VertexLayout::Element::Size() const noexcept(!IS_DEBUG)
	{
		return SizeOf(mType);
	}

	VertexLayout::ElementType VertexLayout::Element::GetType() const noexcept
	{
		return mType;
	}

	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noexcept(!IS_DEBUG)
	{
		switch (mType)
		{
		case ElementType::Position2D: return GenerateDesc<ElementType::Position2D>(GetOffset());
		case ElementType::Position3D: return GenerateDesc<ElementType::Position3D>(GetOffset());
		case ElementType::Texture2D: return GenerateDesc<ElementType::Texture2D>(GetOffset());
		case ElementType::Normal: return GenerateDesc<ElementType::Normal>(GetOffset());
		case ElementType::Float3Color: return GenerateDesc<ElementType::Float3Color>(GetOffset());
		case ElementType::Float4Color: return GenerateDesc<ElementType::Float4Color>(GetOffset());
		case ElementType::BGRAColor: return GenerateDesc<ElementType::BGRAColor>(GetOffset());
		default:
			assert("Invalid element type" && false);
			return { "INVALID", 0u, DXGI_FORMAT_UNKNOWN, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u };
		}
	}

	constexpr size_t VertexLayout::Element::SizeOf(ElementType type) noexcept(!IS_DEBUG)
	{
		switch (type)
		{
		case ElementType::Position2D: return sizeof(Map<ElementType::Position2D>::SysType);
		case ElementType::Position3D: return sizeof(Map<ElementType::Position3D>::SysType);
		case ElementType::Texture2D: return sizeof(Map<ElementType::Texture2D>::SysType);
		case ElementType::Normal: return sizeof(Map<ElementType::Normal>::SysType);
		case ElementType::Float3Color: return sizeof(Map<ElementType::Float3Color>::SysType);
		case ElementType::Float4Color: return sizeof(Map<ElementType::Float4Color>::SysType);
		case ElementType::BGRAColor: return sizeof(Map<ElementType::BGRAColor>::SysType);
		default:
			assert("Invalid element type" && false);
			return 0u;
		}
	}

	const VertexLayout::Element& VertexLayout::ResolveByIndex(size_t i) const noexcept(!IS_DEBUG)
	{
		return mElements[i];
	}

	VertexLayout& VertexLayout::Append(ElementType type) noexcept(!IS_DEBUG)
	{
		mElements.emplace_back(type, Size());
		return *this;
	}

	size_t VertexLayout::Size() const noexcept(!IS_DEBUG)
	{
		return mElements.empty() ? 0u : mElements.back().GetOffsetAfter();
	}

	size_t VertexLayout::GetElementCount() const noexcept
	{
		return mElements.size();
	}

	std::vector<D3D11_INPUT_ELEMENT_DESC> VertexLayout::GetD3DLayout() const noexcept(!IS_DEBUG)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> ied;
		ied.reserve(GetElementCount());
		for (const auto& e : mElements)
			ied.push_back(e.GetDesc());
		return ied;
	}

	Vertex::Vertex(char* pData, const VertexLayout& layout) noexcept(!IS_DEBUG)
		: mData(pData), mLayout(layout)
	{
		assert(pData != nullptr);
	}

	ConstVertex::ConstVertex(const Vertex& v) noexcept(!IS_DEBUG)
		: mVertex(v)
	{}

	VertexBuffer::VertexBuffer(VertexLayout layout) noexcept(!IS_DEBUG)
		: mLayout(std::move(layout))
	{}

	const char* VertexBuffer::GetData() const noexcept(!IS_DEBUG)
	{
		return mBuffer.data();
	}

	const VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return mLayout;
	}

	size_t VertexBuffer::Size() const noexcept(!IS_DEBUG)
	{
		return mBuffer.size() / mLayout.Size();
	}

	size_t VertexBuffer::SizeBytes() const noexcept(!IS_DEBUG)
	{
		return mBuffer.size();
	}

	Vertex VertexBuffer::Back() noexcept(!IS_DEBUG)
	{
		assert(mBuffer.size() != 0u);
		return Vertex{ mBuffer.data() + mBuffer.size() - mLayout.Size(), mLayout };
	}

	Vertex VertexBuffer::Front() noexcept(!IS_DEBUG)
	{
		assert(mBuffer.size() != 0u);
		return Vertex{ mBuffer.data(), mLayout };
	}

	Vertex VertexBuffer::operator[](size_t i) noexcept(!IS_DEBUG)
	{
		assert(i < Size());
		return Vertex{ mBuffer.data() + mLayout.Size() * i, mLayout };
	}

	ConstVertex VertexBuffer::Back() const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer*>(this)->Back();
	}

	ConstVertex VertexBuffer::Front() const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer*>(this)->Front();
	}

	ConstVertex VertexBuffer::operator[](size_t i) const noexcept(!IS_DEBUG)
	{
		return const_cast<VertexBuffer&>(*this)[i];
	}
}