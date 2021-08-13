#pragma once

#include <DirectXMath.h>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#define RESOLVE_BASE(eltype) \
virtual size_t Resolve ## eltype() const noexcept(!IS_DEBUG) \
{ \
	assert(false && "Cannot resolve to " #eltype); \
	return 0u; \
}

#define LEAF_ELEMENT(eltype, systype) \
class eltype : public LayoutElement \
{ \
public: \
	using LayoutElement::LayoutElement; \
	size_t Resolve ## eltype() const noexcept(!IS_DEBUG) override final \
	{ \
		return GetOffsetBegin(); \
	} \
	size_t GetOffsetEnd() const noexcept override final \
	{ \
		return GetOffsetBegin() + sizeof(systype); \
	} \
};

#define REF_CONVERSION(eltype, systype) \
operator systype& () noexcept(!IS_DEBUG) \
{ \
	return *reinterpret_cast<systype*>(mBytes + mLayout->Resolve ## eltype()); \
} \
systype& operator=(const systype& rhs) noexcept(!IS_DEBUG) \
{ \
	return static_cast<systype&>(*this) = rhs; \
}

namespace Dcb
{
	class Struct;

	class LayoutElement
	{
	public:
		LayoutElement(size_t offset);

		virtual LayoutElement& operator[](const wchar_t* key);
		virtual const LayoutElement& operator[](const wchar_t* key) const;

		size_t GetOffsetBegin() const noexcept;
		virtual size_t GetOffsetEnd() const noexcept = 0;

		template<typename T>
		Struct& Add(const std::wstring& key) noexcept(!IS_DEBUG);

		RESOLVE_BASE(Float3);
		RESOLVE_BASE(Float);

	private:
		size_t mOffset;
	};

	LEAF_ELEMENT(Float3, DirectX::XMFLOAT3);
	LEAF_ELEMENT(Float, float);

	class Struct : public LayoutElement
	{
	public:
		using LayoutElement::LayoutElement;

		LayoutElement& operator[](const wchar_t* key) override final;
		const LayoutElement& operator[](const wchar_t* key) const override final;
		size_t GetOffsetEnd() const noexcept override final;

		template<typename T>
		Struct& Add(const std::wstring& name) noexcept(!IS_DEBUG);

	private:
		std::unordered_map<std::wstring, LayoutElement*> mMap;
		std::vector<std::unique_ptr<LayoutElement>> mElements;
	};

	class ElementRef
	{
	public:
		ElementRef(const LayoutElement* pLayout, std::byte* pBytes);
		ElementRef operator[](const wchar_t* key) noexcept(!IS_DEBUG);
		
		REF_CONVERSION(Float3, DirectX::XMFLOAT3);
		REF_CONVERSION(Float, float);

	private:
		const LayoutElement* mLayout;
		std::byte* mBytes;
	};

	class Buffer
	{
	public:
		Buffer(const Struct& pLayout);
		ElementRef operator[](const wchar_t* key) noexcept(!IS_DEBUG);

	private:
		const Struct* mLayout;
		std::vector<std::byte> mBytes;
	};

	template<typename T>
	inline Struct& Struct::Add(const std::wstring& name) noexcept(!IS_DEBUG)
	{
		mElements.push_back(std::make_unique<T>(GetOffsetEnd()));
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
}
