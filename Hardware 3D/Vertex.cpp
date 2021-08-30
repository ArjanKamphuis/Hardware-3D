#define DVTX_SOURCE_FILE
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

	template<VertexLayout::ElementType Type>
	struct DescGenerate
	{
		static constexpr D3D11_INPUT_ELEMENT_DESC Exec(size_t offset) noexcept
		{
			return {
				VertexLayout::Map<Type>::Semantic, 0u,
				VertexLayout::Map<Type>::DXGIFormat, 0u,
				static_cast<UINT>(offset), D3D11_INPUT_PER_VERTEX_DATA, 0u
			};
		}
	};

	D3D11_INPUT_ELEMENT_DESC VertexLayout::Element::GetDesc() const noexcept(!IS_DEBUG)
	{
		return Bridge<DescGenerate>(mType, GetOffset());
	}

	template<VertexLayout::ElementType Type>
	struct CodeLookup
	{
		static constexpr const wchar_t* const Exec() noexcept
		{
			return VertexLayout::Map<Type>::Code;
		}
	};

	const wchar_t* VertexLayout::Element::GetCode() const noexcept
	{
		return Bridge<CodeLookup>(mType);
	}

	template<VertexLayout::ElementType Type>
	struct SysSizeLookup
	{
		static constexpr size_t Exec() noexcept
		{
			return sizeof(VertexLayout::Map<Type>::SysType);
		}
	};

	constexpr size_t VertexLayout::Element::SizeOf(ElementType type) noexcept(!IS_DEBUG)
	{
		return Bridge<SysSizeLookup>(type);
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

	std::wstring VertexLayout::GetCode() const noexcept(!IS_DEBUG)
	{
		std::wstring code;
		for (const Element& e : mElements)
			code += e.GetCode();
		return code;
	}

	Vertex::Vertex(char* pData, const VertexLayout& layout) noexcept(!IS_DEBUG)
		: mData(pData), mLayout(layout)
	{
		assert(pData != nullptr);
	}

	ConstVertex::ConstVertex(const Vertex& v) noexcept(!IS_DEBUG)
		: mVertex(v)
	{}

	VertexBuffer::VertexBuffer(VertexLayout layout, size_t size) noexcept(!IS_DEBUG)
		: mLayout(std::move(layout))
	{
		Resize(size);
	}

	template<VertexLayout::ElementType Type>
	struct AttributeAiMeshFill
	{
		static constexpr void Exec(VertexBuffer* pBuffer, const aiMesh& mesh) noexcept(!IS_DEBUG)
		{
			for (size_t i = 0; i < mesh.mNumVertices; i++)
				(*pBuffer)[i].Attr<Type>() = VertexLayout::Map<Type>::Extract(mesh, i);
		}
	};

	VertexBuffer::VertexBuffer(VertexLayout layout, const aiMesh& mesh)
		: mLayout(std::move(layout))
	{
		Resize(mesh.mNumVertices);
		for (size_t i = 0; i < mLayout.GetElementCount(); i++)
			VertexLayout::Bridge<AttributeAiMeshFill>(mLayout.ResolveByIndex(i).GetType(), this, mesh);
	}

	const char* VertexBuffer::GetData() const noexcept(!IS_DEBUG)
	{
		return mBuffer.data();
	}

	const VertexLayout& VertexBuffer::GetLayout() const noexcept
	{
		return mLayout;
	}

	void VertexBuffer::Resize(size_t newSize) noexcept(!IS_DEBUG)
	{
		const size_t size = Size();
		if (size < newSize)
			mBuffer.resize(mBuffer.size() + mLayout.Size() * (newSize - size));
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
