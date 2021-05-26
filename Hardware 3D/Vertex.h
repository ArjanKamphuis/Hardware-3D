#pragma once

#include <DirectXPackedVector.h>
#include "Graphics.h"

namespace Dvtx
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
			static constexpr const wchar_t* Code = L"P2";
		};
		template<> struct Map<ElementType::Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* Semantic = "POSITION";
			static constexpr const wchar_t* Code = L"P3";
		};
		template<> struct Map<ElementType::Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* Semantic = "TEXCOORD";
			static constexpr const wchar_t* Code = L"T2";
		};
		template<> struct Map<ElementType::Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* Semantic = "NORMAL";
			static constexpr const wchar_t* Code = L"N";
		};
		template<> struct Map<ElementType::Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* Semantic = "COLOR";
			static constexpr const wchar_t* Code = L"C3";
		};
		template<> struct Map<ElementType::Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* Semantic = "COLOR";
			static constexpr const wchar_t* Code = L"C4";
		};
		template<> struct Map<ElementType::BGRAColor>
		{
			using SysType = DirectX::PackedVector::XMCOLOR;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* Semantic = "COLOR";
			static constexpr const wchar_t* Code = L"C8";
		};

		class Element
		{
		public:
			Element(ElementType type, size_t offset);

			size_t GetOffsetAfter() const noexcept(!IS_DEBUG);
			size_t GetOffset() const;
			size_t Size() const noexcept(!IS_DEBUG);
			ElementType GetType() const noexcept;
			D3D11_INPUT_ELEMENT_DESC GetDesc() const noexcept(!IS_DEBUG);
			const wchar_t* GetCode() const noexcept;

			static constexpr size_t SizeOf(ElementType type) noexcept(!IS_DEBUG);

		private:
			template<ElementType Type>
			static constexpr D3D11_INPUT_ELEMENT_DESC GenerateDesc(size_t offset) noexcept
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

		const Element& ResolveByIndex(size_t i) const noexcept(!IS_DEBUG);

		VertexLayout& Append(ElementType type) noexcept(!IS_DEBUG);

		size_t Size() const noexcept(!IS_DEBUG);
		size_t GetElementCount() const noexcept;
		std::vector<D3D11_INPUT_ELEMENT_DESC> GetD3DLayout() const noexcept(!IS_DEBUG);
		std::wstring GetCode() const noexcept(!IS_DEBUG);

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
		Vertex(char* pData, const VertexLayout& layout) noexcept(!IS_DEBUG);

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
		ConstVertex(const Vertex& v) noexcept(!IS_DEBUG);

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
		VertexBuffer(VertexLayout layout) noexcept(!IS_DEBUG);

		const char* GetData() const noexcept(!IS_DEBUG);
		const VertexLayout& GetLayout() const noexcept;
		size_t Size() const noexcept(!IS_DEBUG);
		size_t SizeBytes() const noexcept(!IS_DEBUG);

		template<typename ...Params>
		void EmplaceBack(Params&&... params) noexcept(!IS_DEBUG)
		{
			assert(sizeof...(params) == mLayout.GetElementCount() && "Param count doesn't match number of vertexd elements");
			mBuffer.resize(mBuffer.size() + mLayout.Size());
			Back().SetAttributeByIndex(0u, std::forward<Params>(params)...);
		}

		Vertex Back() noexcept(!IS_DEBUG);
		Vertex Front() noexcept(!IS_DEBUG);

		Vertex operator[](size_t i) noexcept(!IS_DEBUG);

		ConstVertex Back() const noexcept(!IS_DEBUG);
		ConstVertex Front() const noexcept(!IS_DEBUG);
		ConstVertex operator[](size_t i) const noexcept(!IS_DEBUG);

	private:
		std::vector<char> mBuffer;
		VertexLayout mLayout;
	};
}
