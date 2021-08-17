#pragma once

#include <algorithm>
#include <DirectXMath.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "ChiliWin.h"

#define DCB_RESOLVE_BASE(eltype) \
virtual size_t Resolve ## eltype() const noexcept(!IS_DEBUG);

#define DCB_LEAF_ELEMENT_IMPL(eltype, systype, hlslSize) \
class eltype : public LayoutElement \
{ \
	friend LayoutElement; \
public: \
	using SystemType = systype; \
	size_t Resolve ## eltype() const noexcept(!IS_DEBUG) override final; \
	size_t GetOffsetEnd() const noexcept override final; \
	std::wstring GetSignature() const noexcept(!IS_DEBUG) override final; \
protected: \
	size_t Finalize(size_t offset) override; \
	size_t ComputeSize() const noexcept(!IS_DEBUG) override final; \
};
#define DCB_LEAF_ELEMENT(eltype, systype) DCB_LEAF_ELEMENT_IMPL(eltype, systype, sizeof(systype))

#define DCB_REF_CONVERSION(eltype, ...) \
operator __VA_ARGS__ eltype::SystemType& () noexcept(!IS_DEBUG);
#define DCB_REF_ASSIGN(eltype) \
eltype::SystemType& operator=(const eltype::SystemType& rhs) noexcept(!IS_DEBUG);
#define DCB_REF_NONCONST(eltype) DCB_REF_CONVERSION(eltype) DCB_REF_ASSIGN(eltype)
#define DCB_REF_CONST(eltype) DCB_REF_CONVERSION(eltype)

#define DCB_PTR_CONVERSION(eltype, ...) \
operator __VA_ARGS__ eltype::SystemType*() noexcept(!IS_DEBUG);

namespace Dcb
{
	class LayoutElement
	{
		friend class Layout;
		friend class Array;
		friend class Struct;

	public:
		virtual ~LayoutElement();

		virtual LayoutElement& operator[](const std::wstring& key);
		virtual const LayoutElement& operator[](const std::wstring& key) const;
		virtual LayoutElement& T();
		virtual const LayoutElement& T() const;

		virtual bool Exists() const noexcept;

		size_t GetOffsetBegin() const noexcept;
		virtual size_t GetOffsetEnd() const noexcept = 0;
		size_t GetSizeInBytes() const noexcept;
		virtual std::wstring GetSignature() const noexcept(!IS_DEBUG) = 0;

		template<typename T>
		LayoutElement& Add(const std::wstring& key) noexcept(!IS_DEBUG);
		template<typename T>
		LayoutElement& Set(size_t size) noexcept(!IS_DEBUG);

		static size_t GetNextBoundaryOffset(size_t offset);

		DCB_RESOLVE_BASE(Matrix);
		DCB_RESOLVE_BASE(Float4);
		DCB_RESOLVE_BASE(Float3);
		DCB_RESOLVE_BASE(Float2);
		DCB_RESOLVE_BASE(Float);
		DCB_RESOLVE_BASE(Bool);

	protected:
		virtual size_t Finalize(size_t offset) = 0;
		virtual size_t ComputeSize() const noexcept(!IS_DEBUG) = 0;

	protected:
		size_t mOffset = 0u;
	};

	DCB_LEAF_ELEMENT(Matrix, DirectX::XMFLOAT4X4);
	DCB_LEAF_ELEMENT(Float4, DirectX::XMFLOAT4);
	DCB_LEAF_ELEMENT(Float3, DirectX::XMFLOAT3);
	DCB_LEAF_ELEMENT(Float2, DirectX::XMFLOAT2);
	DCB_LEAF_ELEMENT(Float, float);
	DCB_LEAF_ELEMENT_IMPL(Bool, bool, 4u);

	class Struct : public LayoutElement
	{
		friend LayoutElement;
	public:
		LayoutElement& operator[](const std::wstring& key) override final;
		const LayoutElement& operator[](const std::wstring& key) const override final;
		size_t GetOffsetEnd() const noexcept override final;
		std::wstring GetSignature() const noexcept(!IS_DEBUG) override final;

		template<typename T>
		void Add(const std::wstring& name, std::unique_ptr<LayoutElement> pElement) noexcept(!IS_DEBUG);

	protected:
		Struct() = default;
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
		friend LayoutElement;
		
	public:
		LayoutElement& T() override final;
		const LayoutElement& T() const override final;
		size_t GetOffsetEnd() const noexcept override final;
		std::wstring GetSignature() const noexcept(!IS_DEBUG) override final;

		template<typename T>
		void Set(std::unique_ptr<LayoutElement> pElement, size_t size) noexcept(!IS_DEBUG);
		bool IndexInBounds(size_t index) const noexcept;

	protected:
		Array() = default;
		size_t Finalize(size_t offset) override final;
		size_t ComputeSize() const noexcept(!IS_DEBUG) override final;

	private:
		size_t mSize = 0u;
		std::unique_ptr<LayoutElement> mElement;
	};

	class Layout
	{
		friend class LayoutCodex;
		friend class Buffer;

	public:
		Layout();
		Layout(std::shared_ptr<LayoutElement> pLayout);

		LayoutElement& operator[](const std::wstring& key);
		size_t GetSizeInBytes() const noexcept;
		std::wstring GetSignature() const noexcept(!IS_DEBUG);

		template<typename T>
		LayoutElement& Add(const std::wstring& key);
		void Finalize();
		bool IsFinalized() const noexcept;

	private:
		std::shared_ptr<LayoutElement> ShareRoot() const noexcept;

	private:
		bool mFinalized = false;
		std::shared_ptr<LayoutElement> mLayout;
	};

	class ConstElementRef
	{
		friend class ElementRef;
		friend class Buffer;

	public:
		class Ptr
		{
			friend ConstElementRef;

		public:
			DCB_PTR_CONVERSION(Matrix, const);
			DCB_PTR_CONVERSION(Float4, const);
			DCB_PTR_CONVERSION(Float3, const);
			DCB_PTR_CONVERSION(Float2, const);
			DCB_PTR_CONVERSION(Float, const);
			DCB_PTR_CONVERSION(Bool, const);

		private:
			Ptr(ConstElementRef& ref);

		private:
			ConstElementRef& mRef;
		};

	public:
		ConstElementRef operator[](const std::wstring& key) noexcept(!IS_DEBUG);
		ConstElementRef operator[](size_t index) noexcept(!IS_DEBUG);

		Ptr operator&() noexcept(!IS_DEBUG);
		bool Exists() const noexcept;

		DCB_REF_CONST(Matrix);
		DCB_REF_CONST(Float4);
		DCB_REF_CONST(Float3);
		DCB_REF_CONST(Float2);
		DCB_REF_CONST(Float);
		DCB_REF_CONST(Bool);

	private:
		ConstElementRef(const LayoutElement* pLayout, std::byte* pBytes, size_t offset);

	private:
		const LayoutElement* mLayout;
		std::byte* mBytes;
		size_t mOffset;
	};

	class ElementRef
	{
		friend class Buffer;

	public:
		class Ptr
		{
			friend ElementRef;
		public:
			DCB_PTR_CONVERSION(Matrix);
			DCB_PTR_CONVERSION(Float4);
			DCB_PTR_CONVERSION(Float3);
			DCB_PTR_CONVERSION(Float2);
			DCB_PTR_CONVERSION(Float);
			DCB_PTR_CONVERSION(Bool);

		private:
			Ptr(ElementRef& ref);

		private:
			ElementRef& mRef;
		};

	public:
		ElementRef operator[](const std::wstring& key) noexcept(!IS_DEBUG);
		ElementRef operator[](size_t index) noexcept(!IS_DEBUG);

		Ptr operator&() noexcept(!IS_DEBUG);
		operator ConstElementRef() const noexcept;
		bool Exists() const noexcept;

		DCB_REF_NONCONST(Matrix);
		DCB_REF_NONCONST(Float4);
		DCB_REF_NONCONST(Float3);
		DCB_REF_NONCONST(Float2);
		DCB_REF_NONCONST(Float);
		DCB_REF_NONCONST(Bool);

	private:
		ElementRef(const LayoutElement* pLayout, std::byte* pBytes, size_t offset);

	private:
		const LayoutElement* mLayout;
		std::byte* mBytes;
		size_t mOffset;
	};

	class Buffer
	{
	public:
		static Buffer Make(Layout& layout) noexcept(!IS_DEBUG);
		ElementRef operator[](const std::wstring& key) noexcept(!IS_DEBUG);
		ConstElementRef operator[](const std::wstring& key) const noexcept(!IS_DEBUG);

		const std::byte* GetData() const noexcept;
		size_t GetSizeInBytes() const noexcept;
		std::wstring GetSignature() const noexcept(!IS_DEBUG);
		const LayoutElement& GetLayout() const noexcept;
		std::shared_ptr<LayoutElement> ShareLayout() const;

	private:
		Buffer(Layout& layout);
		Buffer(Layout&& layout);

	private:
		std::shared_ptr<LayoutElement> mLayout;
		std::vector<std::byte> mBytes;
	};

	template<typename T>
	inline LayoutElement& LayoutElement::Add(const std::wstring& key) noexcept(!IS_DEBUG)
	{
		Struct* ps = dynamic_cast<Struct*>(this);
		assert(ps != nullptr);
		struct Enabler : public T {};
		ps->Add<T>(key, std::make_unique<Enabler>());
		return *this;
	}

	template<typename T>
	inline LayoutElement& LayoutElement::Set(size_t size) noexcept(!IS_DEBUG)
	{
		Array* pa = dynamic_cast<Array*>(this);
		assert(pa != nullptr);
		struct Enabler : public T {};
		pa->Set<T>(std::make_unique<Enabler>(), size);
		return *this;
	}

	// temporary
	bool ValidateSymbolName(const std::wstring& name) noexcept;

	template<typename T>
	inline void Struct::Add(const std::wstring& name, std::unique_ptr<LayoutElement> pElement) noexcept(!IS_DEBUG)
	{
		assert(ValidateSymbolName(name) && "Invalid symbol name in struct");
		mElements.push_back(std::move(pElement));
		if (!mMap.emplace(name, mElements.back().get()).second)
			assert(false && "Duplicate symbol name in struct");
	}

	template<typename T>
	inline void Array::Set(std::unique_ptr<LayoutElement> pElement, size_t size) noexcept(!IS_DEBUG)
	{
		mElement = std::move(pElement);
		mSize = size;
	}

	template<typename T>
	inline LayoutElement& Layout::Add(const std::wstring& key)
	{
		assert(!mFinalized && "Cannot modify finalized layout");
		return mLayout->Add<T>(key);
	}
}

#undef DCB_RESOLVE_BASE
#undef DCB_LEAF_ELEMENT_IMPL
#undef DCB_LEAF_ELEMENT
#undef DCB_REF_CONVERSION
#undef DCB_REF_ASSIGN
#undef DCB_REF_NONCONST
#undef DCB_REF_CONST
#undef DCB_PTR_CONVERSION
