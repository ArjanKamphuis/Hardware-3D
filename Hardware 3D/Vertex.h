#pragma once

#include <DirectXPackedVector.h>
#include "Graphics.h"

namespace hw3dexp
{
	class VertexLayout
	{
	public:
		enum class ElementType { Position2D, Position3D, Texture2D, Normal, Float3Color, Float4Color, BGRAColor, Count };

		template<ElementType> struct Map;
		template<> struct Map<ElementType::Position2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* Semantic = "POSITION";
		};
		template<> struct Map<ElementType::Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* Semantic = "POSITION";
		};
		template<> struct Map<ElementType::Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* Semantic = "TEXCOORD";
		};
		template<> struct Map<ElementType::Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* Semantic = "NORMAL";
		};
		template<> struct Map<ElementType::Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* Semantic = "COLOR";
		};
		template<> struct Map<ElementType::Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* Semantic = "COLOR";
		};
		template<> struct Map<ElementType::BGRAColor>
		{
			using SysType = DirectX::PackedVector::XMCOLOR;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* Semantic = "COLOR";
		};

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
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept(!IS_DEBUG)
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

			static constexpr size_t SizeOf(ElementType type) noexcept(!IS_DEBUG)
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

		private:
			template<ElementType Type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noexcept(!IS_DEBUG)
			{
				return { Map<Type>::Semantic, 0u, Map<Type>::DXGIFormat, 0u, static_cast<UINT>(offset), D3D11_INPUT_PER_VERTEX_DATA, 0u };
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

		VertexLayout& Append(ElementType type) noexcept(!IS_DEBUG)
		{
			mElements.emplace_back(type, Size());
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
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept(!IS_DEBUG)
		{
			std::vector<D3D11_INPUT_ELEMENT_DESC> ied;
			ied.reserve(GetElementCount());
			for (const auto& e : mElements)
				ied.push_back(e.GetDesc());
			return ied;
		}

	private:
		std::vector<Element> mElements;
	};

	class Vertex
	{
		friend class VertexBuffer;

	public:
		template<VertexLayout::ElementType Type>
		auto& Attr() noexcept(!IS_DEBUG)
		{
			auto pAttribute = mData + mLayout.Resolve<Type>().GetOffset();
			return *reinterpret_cast<typename VertexLayout::Map<Type>::SysType*>(pAttribute);
		}

		template<typename T>
		void SetAttributeByIndex(size_t i, T&& value) noexcept(!IS_DEBUG)
		{
			const auto& element = mLayout.ResolveByIndex(i);
			auto pAttribute = mData + element.GetOffset();

			switch (element.GetType())
			{
				using ElementType = VertexLayout::ElementType;
			case ElementType::Position2D:
				SetAttribute<ElementType::Position2D>(pAttribute, std::forward<T>(value));
				break;
			case ElementType::Position3D:
				SetAttribute<ElementType::Position3D>(pAttribute, std::forward<T>(value));
				break;
			case ElementType::Texture2D:
				SetAttribute<ElementType::Texture2D>(pAttribute, std::forward<T>(value));
				break;
			case ElementType::Normal:
				SetAttribute<ElementType::Normal>(pAttribute, std::forward<T>(value));
				break;
			case ElementType::Float3Color:
				SetAttribute<ElementType::Float3Color>(pAttribute, std::forward<T>(value));
				break;
			case ElementType::Float4Color:
				SetAttribute<ElementType::Float4Color>(pAttribute, std::forward<T>(value));
				break;
			case ElementType::BGRAColor:
				SetAttribute<ElementType::BGRAColor>(pAttribute, std::forward<T>(value));
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

		template<VertexLayout::ElementType DestLayoutType, typename SrcType>
		void SetAttribute(char* pAttribute, SrcType&& value) noexcept(!IS_DEBUG)
		{
			using Dest = typename VertexLayout::Map<DestLayoutType>::SysType;

			if constexpr (std::is_assignable<Dest, SrcType>::value)
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

	class VertexBuffer
	{
	public:
		VertexBuffer(VertexLayout layout) noexcept(!IS_DEBUG)
			: mLayout(std::move(layout))
		{}

		const char* GetData() const noexcept(!IS_DEBUG)
		{
			return mBuffer.data();
		}
		const VertexLayout& GetLayout() const noexcept
		{
			return mLayout;
		}
		size_t Size() const noexcept(!IS_DEBUG)
		{
			return mBuffer.size() / mLayout.Size();
		}
		size_t SizeBytes() const noexcept(!IS_DEBUG)
		{
			return mBuffer.size();
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
			return const_cast<VertexBuffer*>(this)->Back();
		}
		ConstVertex Front() const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer*>(this)->Front();
		}
		ConstVertex operator[](size_t i) const noexcept(!IS_DEBUG)
		{
			return const_cast<VertexBuffer&>(*this)[i];
		}

	private:
		std::vector<char> mBuffer;
		VertexLayout mLayout;
	};
}
