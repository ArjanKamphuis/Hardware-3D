#pragma once

#include <DirectXMath.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "ChiliWin.h"

#define RESOLVE_BASE(eltype) \
virtual size_t Resolve ## eltype() const noexcept(!IS_DEBUG) \
{ \
	assert(false && "Cannot resolve to " #eltype); \
	return 0u; \
}

#define LEAF_ELEMENT_IMPL(eltype, systype, hlslSize) \
class eltype : public LayoutElement \
{ \
public: \
	using SystemType = systype; \
	size_t Resolve ## eltype() const noexcept(!IS_DEBUG) override final \
	{ \
		return GetOffsetBegin(); \
	} \
	size_t GetOffsetEnd() const noexcept override final \
	{ \
		return GetOffsetBegin() + ComputeSize(); \
	} \
protected: \
	size_t Finalize(size_t offset) override \
	{ \
		mOffset = offset; \
		return offset + ComputeSize(); \
	} \
	size_t ComputeSize() const noexcept(!IS_DEBUG) override final \
	{ \
		return hlslSize; \
	} \
};
#define LEAF_ELEMENT(eltype, systype) LEAF_ELEMENT_IMPL(eltype, systype, sizeof(systype))

#define REF_CONVERSION(eltype) \
operator eltype::SystemType& () noexcept(!IS_DEBUG) \
{ \
	return *reinterpret_cast<eltype::SystemType*>(mBytes + mOffset + mLayout->Resolve ## eltype()); \
} \
eltype::SystemType& operator=(const eltype::SystemType& rhs) noexcept(!IS_DEBUG) \
{ \
	return static_cast<eltype::SystemType&>(*this) = rhs; \
}

#define PTR_CONVERSION(eltype) \
operator eltype::SystemType*() noexcept(!IS_DEBUG) \
{ \
	return &static_cast<eltype::SystemType&>(mRef); \
}

namespace Dcb
{
	class Struct;
	class Array;
	class Layout;

	class LayoutElement
	{
		friend class Layout;
		friend class Array;
		friend class Struct;

	public:
		virtual ~LayoutElement();

		virtual LayoutElement& operator[](const wchar_t* key);
		virtual const LayoutElement& operator[](const wchar_t* key) const;
		virtual LayoutElement& T();
		virtual const LayoutElement& T() const;

		size_t GetOffsetBegin() const noexcept;
		virtual size_t GetOffsetEnd() const noexcept = 0;
		size_t GetSizeInBytes() const noexcept;

		template<typename T>
		Struct& Add(const std::wstring& key) noexcept(!IS_DEBUG);
		template<typename T>
		Array& Set(size_t size) noexcept(!IS_DEBUG);

		static size_t GetNextBoundaryOffset(size_t offset);

		RESOLVE_BASE(Matrix);
		RESOLVE_BASE(Float4);
		RESOLVE_BASE(Float3);
		RESOLVE_BASE(Float2);
		RESOLVE_BASE(Float);
		RESOLVE_BASE(Bool);

	protected:
		virtual size_t Finalize(size_t offset) = 0;
		virtual size_t ComputeSize() const noexcept(!IS_DEBUG) = 0;

	protected:
		size_t mOffset = 0u;
	};

	LEAF_ELEMENT(Matrix, DirectX::XMFLOAT4X4);
	LEAF_ELEMENT(Float4, DirectX::XMFLOAT4);
	LEAF_ELEMENT(Float3, DirectX::XMFLOAT3);
	LEAF_ELEMENT(Float2, DirectX::XMFLOAT2);
	LEAF_ELEMENT(Float, float);
	LEAF_ELEMENT_IMPL(Bool, bool, 4u);

	class Struct : public LayoutElement
	{
	public:
		LayoutElement& operator[](const wchar_t* key) override final;
		const LayoutElement& operator[](const wchar_t* key) const override final;
		size_t GetOffsetEnd() const noexcept override final;

		template<typename T>
		Struct& Add(const std::wstring& name) noexcept(!IS_DEBUG);

	protected:
		size_t Finalize(size_t offset) override final;
		size_t ComputeSize() const noexcept(!IS_DEBUG) override final;

	private:
		static size_t CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept;

	private:
		std::unordered_map<std::wstring, LayoutElement*> mMap;
		std::vector<std::unique_ptr<LayoutElement>> mElements;
	};

	class Array : public LayoutElement
	{
	public:
		LayoutElement& T() override final;
		const LayoutElement& T() const override final;
		size_t GetOffsetEnd() const noexcept override final;

		template<typename T>
		Array& Set(size_t size) noexcept(!IS_DEBUG);

	protected:
		size_t Finalize(size_t offset) override final;
		size_t ComputeSize() const noexcept(!IS_DEBUG) override final;

	private:
		size_t mSize = 0u;
		std::unique_ptr<LayoutElement> mElement;
	};

	class Layout
	{
	public:
		Layout();
		Layout(std::shared_ptr<LayoutElement> pLayout);

		LayoutElement& operator[](const wchar_t* key);
		size_t GetSizeInBytes() const noexcept;

		template<typename T>
		LayoutElement& Add(const wchar_t* key);
		std::shared_ptr<LayoutElement> Finalize();

	private:
		bool mFinalized = false;
		std::shared_ptr<LayoutElement> mLayout;
	};

	class ElementRef
	{
	public:
		class Ptr
		{
		public:
			Ptr(ElementRef& ref);

			PTR_CONVERSION(Matrix);
			PTR_CONVERSION(Float4);
			PTR_CONVERSION(Float3);
			PTR_CONVERSION(Float2);
			PTR_CONVERSION(Float);
			PTR_CONVERSION(Bool);

		private:
			ElementRef& mRef;
		};

	public:
		ElementRef(const LayoutElement* pLayout, std::byte* pBytes, size_t offset);
		ElementRef operator[](const wchar_t* key) noexcept(!IS_DEBUG);
		ElementRef operator[](size_t index) noexcept(!IS_DEBUG);

		Ptr operator&() noexcept(!IS_DEBUG);
		
		REF_CONVERSION(Matrix);
		REF_CONVERSION(Float4);
		REF_CONVERSION(Float3);
		REF_CONVERSION(Float2);
		REF_CONVERSION(Float);
		REF_CONVERSION(Bool);

	private:
		const LayoutElement* mLayout;
		std::byte* mBytes;
		size_t mOffset;
	};

	class Buffer
	{
	public:
		Buffer(Layout& layout);
		ElementRef operator[](const wchar_t* key) noexcept(!IS_DEBUG);

		const std::byte* GetData() const noexcept;
		size_t GetSizeInBytes() const noexcept;
		const LayoutElement& GetLayout() const noexcept;
		std::shared_ptr<LayoutElement> CloneLayout() const;

	private:
		std::shared_ptr<Struct> mLayout;
		std::vector<std::byte> mBytes;
	};

	template<typename T>
	inline Struct& Struct::Add(const std::wstring& name) noexcept(!IS_DEBUG)
	{
		mElements.push_back(std::make_unique<T>());
		if (!mMap.emplace(name, mElements.back().get()).second)
			assert(false);
		return *this;
	}

	template<typename T>
	inline Struct& LayoutElement::Add(const std::wstring& key) noexcept(!IS_DEBUG)
	{
		Struct* ps = dynamic_cast<Struct*>(this);
		assert(ps != nullptr);
		return ps->Add<T>(key);
	}

	template<typename T>
	inline Array& LayoutElement::Set(size_t size) noexcept(!IS_DEBUG)
	{
		Array* pa = dynamic_cast<Array*>(this);
		assert(pa != nullptr);
		return pa->Set<T>(size);
	}

	template<typename T>
	inline Array& Array::Set(size_t size) noexcept(!IS_DEBUG)
	{
		mElement = std::make_unique<T>();
		mSize = size;
		return *this;
	}

	template<typename T>
	inline LayoutElement& Layout::Add(const wchar_t* key)
	{
		assert(!mFinalized && "Cannot modify finalized layout");
		return mLayout->Add<T>(key);
	}
}
