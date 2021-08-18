#pragma once

#include <algorithm>
#include <DirectXMath.h>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#define LEAF_ELEMENT_TYPES \
	X(Float) \
	X(Float2) \
	X(Float3) \
	X(Float4) \
	X(Matrix) \
	X(Bool)

namespace Dcb
{
	enum class Type
	{
		#define X(el) el,
		LEAF_ELEMENT_TYPES
		#undef X
		Struct,
		Array,
		Empty
	};

	template<Type type>
	struct Map
	{
		static constexpr bool Valid = false;
	};
	template<> struct Map<Type::Float>
	{
		using SysType = float;
		static constexpr size_t HlslSize = sizeof(SysType);
		static constexpr const wchar_t* Code = L"F1";
		static constexpr bool Valid = true;
	};
	template<> struct Map<Type::Float2>
	{
		using SysType = DirectX::XMFLOAT2;
		static constexpr size_t HlslSize = sizeof(SysType);
		static constexpr const wchar_t* Code = L"F2";
		static constexpr bool Valid = true;
	};
	template<> struct Map<Type::Float3>
	{
		using SysType = DirectX::XMFLOAT3;
		static constexpr size_t HlslSize = sizeof(SysType);
		static constexpr const wchar_t* Code = L"F3";
		static constexpr bool Valid = true;
	};
	template<> struct Map<Type::Float4>
	{
		using SysType = DirectX::XMFLOAT4;
		static constexpr size_t HlslSize = sizeof(SysType);
		static constexpr const wchar_t* Code = L"F4";
		static constexpr bool Valid = true;
	};
	template<> struct Map<Type::Matrix>
	{
		using SysType = DirectX::XMFLOAT4X4;
		static constexpr size_t HlslSize = sizeof(SysType);
		static constexpr const wchar_t* Code = L"M4";
		static constexpr bool Valid = true;
	};
	template<> struct Map<Type::Bool>
	{
		using SysType = bool;
		static constexpr size_t HlslSize = sizeof(SysType);
		static constexpr const wchar_t* Code = L"BL";
		static constexpr bool Valid = true;
	};

#define X(el) static_assert(Map<Type::el>::Valid, "Missing map implementation for " #el);
	LEAF_ELEMENT_TYPES
#undef X

	template<typename T>
	struct ReverseMap
	{
		static constexpr bool Valid = false;
	};
#define X(el) \
	template<> struct ReverseMap<typename Map<Type::el>::SysType> \
	{ \
		static constexpr Type Type = Type::el; \
		static constexpr bool Valid = true; \
	};
	LEAF_ELEMENT_TYPES
#undef X

	class LayoutElement
	{
	private:
		struct ExtraDataBase
		{
			virtual ~ExtraDataBase() = default;
		};

		friend class RawLayout;
		friend struct ExtraData;

	public:
		std::pair<size_t, const LayoutElement*> CalculateIndexingOffset(size_t offset, size_t index) const noexcept(!IS_DEBUG);

		LayoutElement& operator[](const std::wstring& key) noexcept(!IS_DEBUG);
		const LayoutElement& operator[](const std::wstring& key) const noexcept(!IS_DEBUG);
		LayoutElement& T() noexcept(!IS_DEBUG);
		const LayoutElement& T() const noexcept(!IS_DEBUG);

		bool Exists() const noexcept;

		size_t GetOffsetBegin() const noexcept(!IS_DEBUG);
		size_t GetOffsetEnd() const noexcept(!IS_DEBUG);
		size_t GetSizeInBytes() const noexcept(!IS_DEBUG);
		std::wstring GetSignature() const noexcept(!IS_DEBUG);

		LayoutElement& Add(Type addedType, std::wstring name) noexcept(!IS_DEBUG);
		template<Type addedType>
		LayoutElement& Add(std::wstring key) noexcept(!IS_DEBUG);

		LayoutElement& Set(Type addedType, size_t size) noexcept(!IS_DEBUG);
		template<Type addedType>
		LayoutElement& Set(size_t size) noexcept(!IS_DEBUG);

		template<typename T>
		size_t Resolve() const noexcept(!IS_DEBUG);

	private:
		LayoutElement() noexcept = default;
		LayoutElement(Type type) noexcept;

		std::wstring GetSignatureForStruct() const noexcept(!IS_DEBUG);
		std::wstring GetSignatureForArray() const noexcept(!IS_DEBUG);

		size_t Finalize(size_t offset) noexcept(!IS_DEBUG);
		size_t FinalizeForStruct(size_t offset) noexcept(!IS_DEBUG);
		size_t FinalizeForArray(size_t offset) noexcept(!IS_DEBUG);

		static LayoutElement& GetEmptyElement() noexcept;
		static size_t AdvanceToBoundary(size_t offset) noexcept;
		static bool CrossesBoundary(size_t offset, size_t size) noexcept;
		static size_t AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept;
		static bool ValidateSymbolName(const std::wstring& name) noexcept;

	private:
		std::optional<size_t> mOffset;
		Type mType = Type::Empty;
		std::unique_ptr<ExtraDataBase> mExtraData;
	};

	class Layout
	{
		friend class LayoutCodex;
		friend class Buffer;

	public:
		size_t GetSizeInBytes() const noexcept;
		std::wstring GetSignature() const noexcept(!IS_DEBUG);

	protected:
		Layout(std::shared_ptr<LayoutElement> pRoot) noexcept;

	protected:
		std::shared_ptr<LayoutElement> mRoot;
	};

	class RawLayout : public Layout
	{
		friend class LayoutCodex;

	public:
		RawLayout() noexcept;

		LayoutElement& operator[](const std::wstring& key) noexcept(!IS_DEBUG);

		template<Type type>
		LayoutElement& Add(const std::wstring& key) noexcept(!IS_DEBUG);

	private:
		void ClearRoot() noexcept;
		std::shared_ptr<LayoutElement> DeliverRoot() noexcept;
	};

	class CookedLayout : public Layout
	{
		friend class LayoutCodex;
		friend class Buffer;

	public:
		const LayoutElement& operator[](const std::wstring& key) const noexcept(!IS_DEBUG);
		std::shared_ptr<LayoutElement> ShareRoot() const noexcept;

	private:
		CookedLayout(std::shared_ptr<LayoutElement> pRoot) noexcept;
		std::shared_ptr<LayoutElement> RelinquishRoot() noexcept;
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
			template<typename T>
			operator const T*() const noexcept(!IS_DEBUG);

		private:
			Ptr(const ConstElementRef* ref) noexcept;

		private:
			const ConstElementRef* mRef;
		};

	public:
		ConstElementRef operator[](const std::wstring& key) noexcept(!IS_DEBUG);
		ConstElementRef operator[](size_t index) noexcept(!IS_DEBUG);

		Ptr operator&() noexcept(!IS_DEBUG);
		bool Exists() const noexcept;

		template<typename T>
		operator const T&() const noexcept(!IS_DEBUG);

	private:
		ConstElementRef(const LayoutElement* pLayout, const std::byte* pBytes, size_t offset) noexcept;

	private:
		const LayoutElement* mLayout;
		const std::byte* mBytes;
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
			template<typename T>
			operator T* () const noexcept(!IS_DEBUG);

		private:
			Ptr(ElementRef* ref) noexcept;

		private:
			ElementRef* mRef;
		};

	public:
		ElementRef operator[](const std::wstring& key) const noexcept(!IS_DEBUG);
		ElementRef operator[](size_t index) const noexcept(!IS_DEBUG);

		Ptr operator&() const noexcept(!IS_DEBUG);
		operator ConstElementRef() const noexcept;
		bool Exists() const noexcept;
		template<typename S>
		bool SetIfExists(const S& val) noexcept(!IS_DEBUG);

		template<typename T>
		operator T& () const noexcept(!IS_DEBUG);
		template<typename T>
		T& operator=(const T& rhs) const noexcept(!IS_DEBUG);

	private:
		ElementRef(const LayoutElement* pLayout, std::byte* pBytes, size_t offset) noexcept;

	private:
		const LayoutElement* mLayout;
		std::byte* mBytes;
		size_t mOffset;
	};

	class Buffer
	{
	public:
		Buffer(RawLayout&& layout) noexcept;
		Buffer(const CookedLayout& layout) noexcept;
		Buffer(const Buffer& rhs) noexcept;
		Buffer(Buffer&& rhs) noexcept;

		ElementRef operator[](const std::wstring& key) noexcept(!IS_DEBUG);
		ConstElementRef operator[](const std::wstring& key) const noexcept(!IS_DEBUG);

		const std::byte* GetData() const noexcept;
		size_t GetSizeInBytes() const noexcept;
		const LayoutElement& GetLayoutRootElement() const noexcept;
		std::shared_ptr<LayoutElement> ShareLayoutRoot() const noexcept;

		void CopyFrom(const Buffer& other) noexcept(!IS_DEBUG);

	private:
		std::shared_ptr<LayoutElement> mLayoutRoot;
		std::vector<std::byte> mBytes;
	};

	template<Type addedType>
	inline LayoutElement& LayoutElement::Add(std::wstring key) noexcept(!IS_DEBUG)
	{
		return Add(addedType, std::move(key));
	}

	template<Type addedType>
	inline LayoutElement& LayoutElement::Set(size_t size) noexcept(!IS_DEBUG)
	{
		return Set(addedType, size);
	}

	template<typename T>
	inline size_t LayoutElement::Resolve() const noexcept(!IS_DEBUG)
	{
		switch (mType)
		{
			#define X(el) case Type::el: assert(typeid(Map<Type::el>::SysType) == typeid(T)); return *mOffset;
			LEAF_ELEMENT_TYPES
			#undef X
			default:
				assert("Tried to resolve non-leaf element" && false);
				return 0u;
		}
	}

	template<Type type>
	inline LayoutElement& RawLayout::Add(const std::wstring& key) noexcept(!IS_DEBUG)
	{
		return mRoot->Add<type>(key);
	}

	template<typename T>
	inline ConstElementRef::Ptr::operator const T* () const noexcept(!IS_DEBUG)
	{
		static_assert(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported SysType used in pointer conversion");
		return &static_cast<const T&>(*mRef);
	}

	template<typename T>
	inline ConstElementRef::operator const T&() const noexcept(!IS_DEBUG)
	{
		static_assert(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported SysType used in conversion");
		return *reinterpret_cast<const T*>(mBytes + mOffset + mLayout->Resolve<T>());
	}

	template<typename T>
	inline ElementRef::Ptr::operator T*() const noexcept(!IS_DEBUG)
	{
		static_assert(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported SysType used in pointer conversion");
		return &static_cast<T&>(*mRef);
	}

	template<typename S>
	inline bool Dcb::ElementRef::SetIfExists(const S& val) noexcept(!IS_DEBUG)
	{
		if (Exists())
		{
			*this = val;
			return true;
		}
		return false;
	}

	template<typename T>
	inline ElementRef::operator T&() const noexcept(!IS_DEBUG)
	{
		static_assert(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported SysType used in conversion");
		return *reinterpret_cast<T*>(mBytes + mOffset + mLayout->Resolve<T>());
	}
	template<typename T>
	inline T& ElementRef::operator=(const T& rhs) const noexcept(!IS_DEBUG)
	{
		static_assert(ReverseMap<std::remove_const_t<T>>::Valid, "Unsupported SysType used in assignment");
		return static_cast<T&>(*this) = rhs;
	}
}

#ifndef DCB_IMPL_SOURCE
#undef LEAF_ELEMENT_TYPES
#endif
