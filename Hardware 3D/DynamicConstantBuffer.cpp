#define DCB_IMPL_SOURCE
#include "DynamicConstantBuffer.h"

#include "LayoutCodex.h"

using namespace DirectX;

namespace Dcb
{
    struct ExtraData
    {
        struct Struct : public LayoutElement::ExtraDataBase
        {
            std::vector<std::pair<std::wstring, LayoutElement>> LayoutElements;
        };
        struct Array : public LayoutElement::ExtraDataBase
        {
            std::optional<LayoutElement> LayoutElement;
            size_t Size = 0u;
        };
    };

    std::pair<size_t, const LayoutElement*> LayoutElement::CalculateIndexingOffset(size_t offset, size_t index) const noexcept(!IS_DEBUG)
    {
        assert("Indexing into non-array" && mType == Type::Array);
        const Dcb::ExtraData::Array& data = static_cast<ExtraData::Array&>(*mExtraData);
        assert(index < data.Size);
        return { offset + data.LayoutElement->GetSizeInBytes() * index, &*data.LayoutElement };
    }

    LayoutElement& LayoutElement::operator[](const std::wstring& key) noexcept(!IS_DEBUG)
    {
        assert("Keying into non-struct" && mType == Type::Struct);
        for (auto& mem : static_cast<ExtraData::Struct&>(*mExtraData).LayoutElements)
            if (mem.first == key)
                return mem.second;
        return GetEmptyElement();
    }

    const LayoutElement& LayoutElement::operator[](const std::wstring& key) const noexcept(!IS_DEBUG)
    {
        return const_cast<LayoutElement&>(*this)[key];
    }

    LayoutElement& LayoutElement::T() noexcept(!IS_DEBUG)
    {
        assert("Accessing T of non-array" && mType == Type::Array);
        return *static_cast<ExtraData::Array&>(*mExtraData).LayoutElement;
    }

    const LayoutElement& LayoutElement::T() const noexcept(!IS_DEBUG)
    {
        return const_cast<LayoutElement&>(*this).T();
    }

    bool LayoutElement::Exists() const noexcept
    {
        return mType != Type::Empty;
    }

    size_t LayoutElement::GetOffsetBegin() const noexcept(!IS_DEBUG)
    {
        return *mOffset;
    }

    size_t LayoutElement::GetOffsetEnd() const noexcept(!IS_DEBUG)
    {
        switch (mType)
        {
            #define X(el) case Type::el : return *mOffset + Map<Type::el>::HlslSize;
            LEAF_ELEMENT_TYPES
            #undef X
            case Type::Struct:
            {
                const ExtraData::Struct& data = static_cast<ExtraData::Struct&>(*mExtraData);
                return AdvanceToBoundary(data.LayoutElements.back().second.GetOffsetEnd());
            }
            case Type::Array:
            {
                const ExtraData::Array& data = static_cast<ExtraData::Array&>(*mExtraData);
                return *mOffset + AdvanceToBoundary(data.LayoutElement->GetSizeInBytes()) * data.Size;
            }
            default:
                assert("Tried to get offset of empty or invalid element" && false);
                return 0u;
        }
    }

    size_t LayoutElement::GetSizeInBytes() const noexcept(!IS_DEBUG)
    {
        return GetOffsetEnd() - GetOffsetBegin();
    }

    std::wstring LayoutElement::GetSignature() const noexcept(!IS_DEBUG)
    {
        switch (mType)
        {
            #define X(el) case Type::el : return Map<Type::el>::Code;
            LEAF_ELEMENT_TYPES
            #undef X
            case Type::Struct: return GetSignatureForStruct();
            case Type::Array: return GetSignatureForArray();
            default:
                assert("Bad type in signature generation" && false);
                return L"???";
        }
    }

    LayoutElement& LayoutElement::Add(Type addedType, std::wstring name) noexcept(!IS_DEBUG)
    {
        assert("Add to non-struct in layout" && mType == Type::Struct);
        assert("Invalid symbol name in struct" && ValidateSymbolName(name));
        ExtraData::Struct& structData = static_cast<ExtraData::Struct&>(*mExtraData);
        for (auto& mem : structData.LayoutElements)
            if (mem.first == name)
                assert("Adding duplicate name to struct" && false);
        structData.LayoutElements.emplace_back(std::move(name), LayoutElement{ addedType });
        return *this;
    }

    LayoutElement& LayoutElement::Set(Type addedType, size_t size) noexcept(!IS_DEBUG)
    {
        assert("Set on non-array in layout" && mType == Type::Array);
        assert(size != 0u);
        ExtraData::Array& arrayData = static_cast<ExtraData::Array&>(*mExtraData);
        arrayData.LayoutElement = { addedType };
        arrayData.Size = size;
        return *this;
    }

    LayoutElement::LayoutElement(Type type) noexcept
        : mType(type)
    {
        assert(type != Type::Empty);
        if (type == Type::Struct)
            mExtraData = std::unique_ptr<ExtraData::Struct>{ new ExtraData::Struct() };
        else if (type == Type::Array)
            mExtraData = std::unique_ptr<ExtraData::Array>{ new ExtraData::Array() };
    }

    std::wstring LayoutElement::GetSignatureForStruct() const noexcept(!IS_DEBUG)
    {
        using namespace std::string_literals;
        auto sig = L"St{"s;
        for (const auto& el : static_cast<ExtraData::Struct&>(*mExtraData).LayoutElements)
            sig += el.first + L":"s + el.second.GetSignature() + L";"s;
        return sig + L"}"s;
    }

    std::wstring LayoutElement::GetSignatureForArray() const noexcept(!IS_DEBUG)
    {
        using namespace std::string_literals;
        const ExtraData::Array& data = static_cast<ExtraData::Array&>(*mExtraData);
        return L"Ar:"s + std::to_wstring(data.Size) + L"{"s + data.LayoutElement->GetSignature() + L"}"s;
    }

    size_t LayoutElement::Finalize(size_t offset) noexcept(!IS_DEBUG)
    {
        switch(mType)
        {
            #define X(el) case Type::el: mOffset = AdvanceIfCrossesBoundary(offset, Map<Type::el>::HlslSize); return *mOffset + Map<Type::el>::HlslSize;
            LEAF_ELEMENT_TYPES
            #undef X
            case Type::Struct: return FinalizeForStruct(offset);
            case Type::Array: return FinalizeForArray(offset);
            default:
                assert("Bad type in size computation" && false);
                return 0u;
        }
    }

    size_t LayoutElement::FinalizeForStruct(size_t offset) noexcept(!IS_DEBUG)
    {
        ExtraData::Struct& data = static_cast<ExtraData::Struct&>(*mExtraData);
        assert(data.LayoutElements.size() != 0u);
        mOffset = AdvanceToBoundary(offset);
        size_t offsetNext = *mOffset;
        for (auto& el : data.LayoutElements)
            offsetNext = el.second.Finalize(offsetNext);
        return offsetNext;
    }

    size_t LayoutElement::FinalizeForArray(size_t offset) noexcept(!IS_DEBUG)
    {
        ExtraData::Array& data = static_cast<ExtraData::Array&>(*mExtraData);
        assert(data.Size != 0u);
        mOffset = AdvanceToBoundary(offset);
        data.LayoutElement->Finalize(*mOffset);
        return GetOffsetEnd();
    }

    LayoutElement& LayoutElement::GetEmptyElement() noexcept
    {
        static LayoutElement empty{};
        return empty;
    }

    size_t LayoutElement::AdvanceToBoundary(size_t offset) noexcept
    {
        return offset + (16u - offset % 16u) % 16u;
    }

    bool LayoutElement::CrossesBoundary(size_t offset, size_t size) noexcept
    {
        const size_t end = offset + size;
        const size_t pageStart = offset / 16u;
        const size_t pageEnd = end / 16u;
        return (pageStart != pageEnd && end % 16u != 0u) || size > 16u;
    }

    size_t LayoutElement::AdvanceIfCrossesBoundary(size_t offset, size_t size) noexcept
    {
        return CrossesBoundary(offset, size) ? AdvanceToBoundary(offset) : offset;
    }

    bool LayoutElement::ValidateSymbolName(const std::wstring& name) noexcept
    {
        return !name.empty() && !std::isdigit(name.front()) &&
            std::all_of(name.begin(), name.end(),
                [](wchar_t c)
                {
                    return std::isalnum(c) || c == L'_';
                }
        );
    }



    size_t Layout::GetSizeInBytes() const noexcept
    {
        return mRoot->GetSizeInBytes();
    }

    std::wstring Layout::GetSignature() const noexcept(!IS_DEBUG)
    {
        return mRoot->GetSignature();
    }

    Layout::Layout(std::shared_ptr<LayoutElement> pRoot) noexcept
        : mRoot{ std::move(pRoot) }
    {
    }


    RawLayout::RawLayout() noexcept
        : Layout(std::shared_ptr<LayoutElement>{ new LayoutElement(Type::Struct) })
    {
    }

    LayoutElement& RawLayout::operator[](const std::wstring& key) noexcept(!IS_DEBUG)
    {
        return (*mRoot)[key];
    }

    void RawLayout::ClearRoot() noexcept
    {
        *this = RawLayout();
    }

    std::shared_ptr<LayoutElement> RawLayout::DeliverRoot() noexcept
    {
        auto temp = std::move(mRoot);
        temp->Finalize(0u);
        ClearRoot();
        return std::move(temp);
    }


    const LayoutElement& CookedLayout::operator[](const std::wstring& key) const noexcept(!IS_DEBUG)
    {
        return (*mRoot)[key];
    }

    std::shared_ptr<LayoutElement> CookedLayout::ShareRoot() const noexcept
    {
        return mRoot;
    }

    CookedLayout::CookedLayout(std::shared_ptr<LayoutElement> pRoot) noexcept
        : Layout(std::move(pRoot))
    {
    }

    std::shared_ptr<LayoutElement> CookedLayout::RelinquishRoot() noexcept
    {
        return std::move(mRoot);
    }



    ConstElementRef::Ptr::Ptr(const ConstElementRef* ref) noexcept
        : mRef(ref)
    {
    }

    ConstElementRef::ConstElementRef(const LayoutElement* pLayout, const std::byte* pBytes, size_t offset) noexcept
        : mLayout(pLayout), mBytes(pBytes), mOffset(offset)
    {
    }

    ConstElementRef ConstElementRef::operator[](const std::wstring& key) noexcept(!IS_DEBUG)
    {
        return { &(*mLayout)[key], mBytes, mOffset };
    }

    ConstElementRef ConstElementRef::operator[](size_t index) noexcept(!IS_DEBUG)
    {
        const auto indexingData = mLayout->CalculateIndexingOffset(mOffset, index);
        return { indexingData.second, mBytes, indexingData.first };
    }

    ConstElementRef::Ptr ConstElementRef::operator&() noexcept(!IS_DEBUG)
    {
        return Ptr{ this };
    }

    bool ConstElementRef::Exists() const noexcept
    {
        return mLayout->Exists();
    }



    ElementRef::Ptr::Ptr(ElementRef* ref) noexcept
        : mRef(ref)
    {
    }

    ElementRef::ElementRef(const LayoutElement* pLayout, std::byte* pBytes, size_t offset) noexcept
        : mLayout(pLayout), mBytes(pBytes), mOffset(offset)
    {
    }

    ElementRef ElementRef::operator[](const std::wstring& key) const noexcept(!IS_DEBUG)
    {
        return { &(*mLayout)[key], mBytes, mOffset };
    }

    ElementRef ElementRef::operator[](size_t index) const noexcept(!IS_DEBUG)
    {
        const auto indexingData = mLayout->CalculateIndexingOffset(mOffset, index);
        return { indexingData.second, mBytes, indexingData.first };
    }

    ElementRef::Ptr ElementRef::operator&() const noexcept(!IS_DEBUG)
    {
        return Ptr{ const_cast<ElementRef*>(this) };
    }

    ElementRef::operator ConstElementRef() const noexcept
    {
        return { mLayout, mBytes, mOffset };
    }

    bool ElementRef::Exists() const noexcept
    {
        return mLayout->Exists();
    }


    

    Buffer::Buffer(RawLayout&& layout) noexcept
        : Buffer(LayoutCodex::Resolve(std::move(layout)))
    {
    }

    Buffer::Buffer(const CookedLayout& layout) noexcept
        : mLayoutRoot(layout.ShareRoot()), mBytes(mLayoutRoot->GetOffsetEnd())
    {
    }

    Buffer::Buffer(const Buffer& rhs) noexcept
        : mLayoutRoot(rhs.mLayoutRoot), mBytes(rhs.mBytes)
    {
    }

    Buffer::Buffer(Buffer&& rhs) noexcept
        : mLayoutRoot(std::move(rhs.mLayoutRoot)), mBytes(std::move(rhs.mBytes))
    {
    }

    ElementRef Buffer::operator[](const std::wstring& key) noexcept(!IS_DEBUG)
    {
        return { &GetLayoutRootElement()[key], mBytes.data(), 0u };
    }

    ConstElementRef Buffer::operator[](const std::wstring& key) const noexcept(!IS_DEBUG)
    {
        return const_cast<Buffer&>(*this)[key];
    }

    const std::byte* Buffer::GetData() const noexcept
    {
        return mBytes.data();
    }

    size_t Buffer::GetSizeInBytes() const noexcept
    {
        return mBytes.size();
    }

    const LayoutElement& Buffer::GetLayoutRootElement() const noexcept
    {
        return *mLayoutRoot;
    }

    std::shared_ptr<LayoutElement> Buffer::ShareLayoutRoot() const noexcept
    {
        return mLayoutRoot;
    }

    void Buffer::CopyFrom(const Buffer& other) noexcept(!IS_DEBUG)
    {
        assert(&GetLayoutRootElement() == &other.GetLayoutRootElement());
        std::copy(other.mBytes.begin(), other.mBytes.end(), mBytes.begin());
    }
}
