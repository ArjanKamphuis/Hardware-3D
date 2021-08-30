#pragma once

#include <DirectXPackedVector.h>
#include <assimp/scene.h>
#include "Graphics.h"

#define DVTX_ELEMENT_AI_EXTRACTOR(member) \
static SysType Extract(const aiMesh& mesh, size_t i) noexcept \
{ \
	return *reinterpret_cast<const SysType*>(&mesh.member[i]); \
}

#define LAYOUT_ELEMENT_TYPES \
	X(Position2D) \
	X(Position3D) \
	X(Texture2D) \
	X(Normal) \
	X(Tangent) \
	X(BiTangent) \
	X(Float3Color) \
	X(Float4Color) \
	X(BGRAColor) \
	X(Count)

namespace Dvtx
{
	class VertexLayout
	{
	public:
		enum class ElementType
		{
			#define X(el) el,
			LAYOUT_ELEMENT_TYPES
			#undef X
		};

		template<ElementType> struct Map;
		template<> struct Map<ElementType::Position2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* Semantic = "POSITION";
			static constexpr const wchar_t* Code = L"P2";
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices);
		};
		template<> struct Map<ElementType::Position3D>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* Semantic = "POSITION";
			static constexpr const wchar_t* Code = L"P3";
			DVTX_ELEMENT_AI_EXTRACTOR(mVertices);
		};
		template<> struct Map<ElementType::Texture2D>
		{
			using SysType = DirectX::XMFLOAT2;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32_FLOAT;
			static constexpr const char* Semantic = "TEXCOORD";
			static constexpr const wchar_t* Code = L"T2";
			DVTX_ELEMENT_AI_EXTRACTOR(mTextureCoords[0]);
		};
		template<> struct Map<ElementType::Normal>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* Semantic = "NORMAL";
			static constexpr const wchar_t* Code = L"N";
			DVTX_ELEMENT_AI_EXTRACTOR(mNormals);
		};
		template<> struct Map<ElementType::Tangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* Semantic = "TANGENT";
			static constexpr const wchar_t* Code = L"Nt";
			DVTX_ELEMENT_AI_EXTRACTOR(mTangents);
		};
		template<> struct Map<ElementType::BiTangent>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* Semantic = "BITANGENT";
			static constexpr const wchar_t* Code = L"Nb";
			DVTX_ELEMENT_AI_EXTRACTOR(mBitangents);
		};
		template<> struct Map<ElementType::Float3Color>
		{
			using SysType = DirectX::XMFLOAT3;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32_FLOAT;
			static constexpr const char* Semantic = "COLOR";
			static constexpr const wchar_t* Code = L"C3";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0]);
		};
		template<> struct Map<ElementType::Float4Color>
		{
			using SysType = DirectX::XMFLOAT4;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R32G32B32A32_FLOAT;
			static constexpr const char* Semantic = "COLOR";
			static constexpr const wchar_t* Code = L"C4";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0]);
		};
		template<> struct Map<ElementType::BGRAColor>
		{
			using SysType = DirectX::PackedVector::XMCOLOR;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
			static constexpr const char* Semantic = "COLOR";
			static constexpr const wchar_t* Code = L"C8";
			DVTX_ELEMENT_AI_EXTRACTOR(mColors[0]);
		};
		template<> struct Map<ElementType::Count>
		{
			using SysType = long double;
			static constexpr DXGI_FORMAT DXGIFormat = DXGI_FORMAT_UNKNOWN;
			static constexpr const char* Semantic = "!INVALID";
			static constexpr const wchar_t* Code = L"!INV";
			DVTX_ELEMENT_AI_EXTRACTOR(mFaces);
		};

		template<template<VertexLayout::ElementType> class F, typename... Args>
		static constexpr auto Bridge(VertexLayout::ElementType type, Args&&... args) noexcept(!IS_DEBUG)
		{
			switch (type)
			{
				#define X(el) case VertexLayout::ElementType::el: return F<VertexLayout::ElementType::el>::Exec(std::forward<Args>(args)...);
				LAYOUT_ELEMENT_TYPES
				#undef X
			}
			assert("Invalid element type" && false);
			return F<VertexLayout::ElementType::Count>::Exec(std::forward<Args>(args)...);
		}

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

		template<ElementType Type>
		bool Has() const noexcept
		{
			for (auto& e : mElements)
				if (e.GetType() == Type)
					return true;
			return false;
		}

	private:
		std::vector<Element> mElements;
	};

	class Vertex
	{
		friend class VertexBuffer;

	private:
		template<VertexLayout::ElementType Type>
		struct AttributeSetting
		{
			template<typename T>
			static constexpr auto Exec(Vertex* pVertex, char* pAttribute, T&& value) noexcept(!IS_DEBUG)
			{
				return pVertex->SetAttribute<Type>(pAttribute, std::forward<T>(value));
			}
		};

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
			VertexLayout::Bridge<AttributeSetting>(element.GetType(), this, pAttribute, std::forward<T>(value));			
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
		VertexBuffer(VertexLayout layout, size_t size = 0u) noexcept(!IS_DEBUG);
		VertexBuffer(VertexLayout layout, const aiMesh& mesh);

		const char* GetData() const noexcept(!IS_DEBUG);
		const VertexLayout& GetLayout() const noexcept;
		void Resize(size_t newSize) noexcept(!IS_DEBUG);
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

#undef DVTX_ELEMENT_AI_EXTRACTOR
#ifndef DVTX_SOURCE_FILE
#undef LAYOUT_ELEMENT_TYPES
#endif
