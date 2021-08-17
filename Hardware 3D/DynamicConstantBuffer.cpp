#include "DynamicConstantBuffer.h"

#include "LayoutCodex.h"

using namespace DirectX;

#define DCB_RESOLVE_BASE(eltype) \
size_t LayoutElement::Resolve ## eltype() const noexcept(!IS_DEBUG) \
{ \
	assert(false && "Cannot resolve to " #eltype); \
	return 0u; \
}

#define DCB_LEAF_ELEMENT_IMPL(eltype, systype, hlslSize) \
size_t eltype::Resolve ## eltype() const noexcept(!IS_DEBUG) \
{ \
	return GetOffsetBegin(); \
} \
size_t eltype::GetOffsetEnd() const noexcept \
{ \
	return GetOffsetBegin() + ComputeSize(); \
} \
std::wstring eltype::GetSignature() const noexcept(!IS_DEBUG) \
{ \
    return L#eltype; \
}\
size_t eltype::Finalize(size_t offset) noexcept(!IS_DEBUG) \
{ \
	mOffset = offset; \
	return offset + ComputeSize(); \
} \
size_t eltype::ComputeSize() const noexcept(!IS_DEBUG) \
{ \
	return hlslSize; \
}
#define DCB_LEAF_ELEMENT(eltype, systype) DCB_LEAF_ELEMENT_IMPL(eltype, systype, sizeof(systype))

#define DCB_REF_CONVERSION(reftype, eltype, ...) \
reftype::operator __VA_ARGS__ eltype::SystemType& () noexcept(!IS_DEBUG) \
{ \
	return *reinterpret_cast<eltype::SystemType*>(mBytes + mOffset + mLayout->Resolve ## eltype()); \
}
#define DCB_REF_ASSIGN(reftype, eltype) \
eltype::SystemType& reftype::operator=(const eltype::SystemType& rhs) noexcept(!IS_DEBUG) \
{ \
	return static_cast<eltype::SystemType&>(*this) = rhs; \
}
#define DCB_REF_NONCONST(reftype, eltype) DCB_REF_CONVERSION(reftype, eltype) DCB_REF_ASSIGN(reftype, eltype)
#define DCB_REF_CONST(reftype, eltype) DCB_REF_CONVERSION(reftype, eltype)

#define DCB_PTR_CONVERSION(reftype, eltype, ...) \
reftype::Ptr::operator __VA_ARGS__ eltype::SystemType*() noexcept(!IS_DEBUG) \
{ \
	return &static_cast<__VA_ARGS__ eltype::SystemType&>(mRef); \
}

namespace Dcb
{
    LayoutElement::~LayoutElement()
    {
    }

    LayoutElement& LayoutElement::operator[](const std::wstring& key) noexcept(!IS_DEBUG)
    {
        assert(false && "Cannot access member on non Struct");
        return *this;
    }

    const LayoutElement& LayoutElement::operator[](const std::wstring& key) const noexcept(!IS_DEBUG)
    {
        assert(false && "Cannot access member on non Struct");
        return *this;
    }

    LayoutElement& LayoutElement::T() noexcept(!IS_DEBUG)
    {
        assert(false);
        return *this;
    }

    const LayoutElement& LayoutElement::T() const noexcept(!IS_DEBUG)
    {
        assert(false);
        return *this;
    }

    bool LayoutElement::Exists() const noexcept
    {
        return true;
    }

    size_t LayoutElement::GetOffsetBegin() const noexcept
    {
        return mOffset;
    }

    size_t LayoutElement::GetSizeInBytes() const noexcept
    {
        return GetOffsetEnd() - GetOffsetBegin();
    }

    size_t LayoutElement::GetNextBoundaryOffset(size_t offset) noexcept
    {
        return offset + (16u - offset % 16) % 16;
    }

    class Empty : public LayoutElement
    {
    public:
        size_t GetOffsetEnd() const noexcept final
        {
            return 0u;
        }
        bool Exists() const noexcept final
        {
            return false;
        }
        std::wstring GetSignature() const noexcept(!IS_DEBUG) final
        {
            assert(false);
            return L"";
        }
    protected:
        size_t Finalize(size_t offset) final
        {
            return 0u;
        }
        size_t ComputeSize() const noexcept(!IS_DEBUG) final
        {
            return 0u;
        }
    private:
        size_t mSize = 0u;
        std::unique_ptr<LayoutElement> mElement;
    } EmptyLayoutElement;

    DCB_RESOLVE_BASE(Matrix);
    DCB_RESOLVE_BASE(Float4);
    DCB_RESOLVE_BASE(Float3);
    DCB_RESOLVE_BASE(Float2);
    DCB_RESOLVE_BASE(Float);
    DCB_RESOLVE_BASE(Bool);

    DCB_LEAF_ELEMENT(Matrix, DirectX::XMFLOAT4X4);
    DCB_LEAF_ELEMENT(Float4, DirectX::XMFLOAT4);
    DCB_LEAF_ELEMENT(Float3, DirectX::XMFLOAT3);
    DCB_LEAF_ELEMENT(Float2, DirectX::XMFLOAT2);
    DCB_LEAF_ELEMENT(Float, float);
    DCB_LEAF_ELEMENT_IMPL(Bool, bool, 4u);



    LayoutElement& Struct::operator[](const std::wstring& key) noexcept(!IS_DEBUG)
    {
        const auto it = mMap.find(key);
        return it == mMap.end() ? EmptyLayoutElement : *it->second;
    }

    const LayoutElement& Struct::operator[](const std::wstring& key) const noexcept(!IS_DEBUG)
    {
        return const_cast<Struct&>(*this)[key];
    }

    size_t Struct::GetOffsetEnd() const noexcept
    {
        return LayoutElement::GetNextBoundaryOffset(mElements.back()->GetOffsetEnd());
    }

    std::wstring Struct::GetSignature() const noexcept(!IS_DEBUG)
    {
        using namespace std::string_literals;
        std::wstring sig = L"Struct{"s;
        for (const auto& el : mElements)
        {
            auto it = std::find_if(mMap.begin(), mMap.end(), 
                [&el](const std::pair<std::wstring, LayoutElement*>& v)
                {
                    return &*el == v.second;
                }
            );
            sig += it->first + L":"s + el->GetSignature() + L";"s;
        }
        return sig + L"}"s;
    }

    size_t Struct::Finalize(size_t offset) noexcept(!IS_DEBUG)
    {
        assert(mElements.size() != 0u);
        mOffset = offset;
        size_t offsetNext = offset;
        for (auto& el : mElements)
            offsetNext = (*el).Finalize(offsetNext);
        return GetOffsetEnd();
    }

    size_t Struct::ComputeSize() const noexcept(!IS_DEBUG)
    {
        size_t offsetNext = 0u;
        for (auto& el : mElements)
        {
            const size_t elSize = el->ComputeSize();
            offsetNext += CalculatePaddingBeforeElement(offsetNext, elSize) + elSize;
        }
        return GetNextBoundaryOffset(offsetNext);
    }

    size_t Struct::CalculatePaddingBeforeElement(size_t offset, size_t size) noexcept
    {
        return offset / 16u != (offset + size - 1) / 16u ? GetNextBoundaryOffset(offset) - offset : offset;
    }
    


    LayoutElement& Array::T() noexcept(!IS_DEBUG)
    {
        return *mElement;
    }

    const LayoutElement& Array::T() const noexcept(!IS_DEBUG)
    {
        return const_cast<Array*>(this)->T();
    }

    size_t Array::GetOffsetEnd() const noexcept
    {
        return GetOffsetBegin() + LayoutElement::GetNextBoundaryOffset(mElement->GetSizeInBytes()) * mSize;
    }

    std::wstring Array::GetSignature() const noexcept(!IS_DEBUG)
    {
        using namespace std::string_literals;
        return L"Array:"s + std::to_wstring(mSize) + L"{"s + T().GetSignature() + L"}"s;
    }

    bool Array::IndexInBounds(size_t index) const noexcept
    {
        return index < mSize;
    }

    size_t Array::Finalize(size_t offset) noexcept(!IS_DEBUG)
    {
        assert(mSize != 0u && mElement);
        mOffset = offset;
        mElement->Finalize(offset);
        return GetOffsetEnd();
    }

    size_t Array::ComputeSize() const noexcept(!IS_DEBUG)
    {
        return LayoutElement::GetNextBoundaryOffset(mElement->ComputeSize()) * mSize;
    }



    size_t Layout::GetSizeInBytes() const noexcept
    {
        return mRoot->GetSizeInBytes();
    }

    std::wstring Layout::GetSignature() const noexcept(!IS_DEBUG)
    {
        return mRoot->GetSignature();
    }

    Layout::Layout() noexcept
    {
        struct Enabler : public Struct {};
        mRoot = std::make_shared<Enabler>();
    }

    Layout::Layout(std::shared_ptr<LayoutElement> pRoot) noexcept
        : mRoot(std::move(pRoot))
    {
    }


    LayoutElement& RawLayout::operator[](const std::wstring& key) noexcept(!IS_DEBUG)
    {
        return (*mRoot)[key];
    }

    std::shared_ptr<LayoutElement> RawLayout::DeliverRoot() noexcept
    {
        auto temp = std::move(mRoot);
        temp->Finalize(0u);
        ClearRoot();
        return std::move(temp);
    }

    void RawLayout::ClearRoot() noexcept
    {
        *this = RawLayout();
    }


    const LayoutElement& CookedLayout::operator[](const std::wstring& key) const noexcept(!IS_DEBUG)
    {
        return (*mRoot)[key];
    }

    CookedLayout::CookedLayout(std::shared_ptr<LayoutElement> pRoot) noexcept
        : Layout(std::move(pRoot))
    {
    }

    std::shared_ptr<LayoutElement> CookedLayout::ShareRoot() const noexcept
    {
        return mRoot;
    }



    ConstElementRef::Ptr::Ptr(ConstElementRef& ref) noexcept
        : mRef(ref)
    {
    }

    DCB_PTR_CONVERSION(ConstElementRef, Matrix, const);
    DCB_PTR_CONVERSION(ConstElementRef, Float4, const);
    DCB_PTR_CONVERSION(ConstElementRef, Float3, const);
    DCB_PTR_CONVERSION(ConstElementRef, Float2, const);
    DCB_PTR_CONVERSION(ConstElementRef, Float, const);
    DCB_PTR_CONVERSION(ConstElementRef, Bool, const);

    ConstElementRef::ConstElementRef(const LayoutElement* pLayout, std::byte* pBytes, size_t offset) noexcept
        : mLayout(pLayout), mBytes(pBytes), mOffset(offset)
    {
    }

    ConstElementRef ConstElementRef::operator[](const std::wstring& key) noexcept(!IS_DEBUG)
    {
        return { &(*mLayout)[key], mBytes, mOffset };
    }

    ConstElementRef ConstElementRef::operator[](size_t index) noexcept(!IS_DEBUG)
    {
        const LayoutElement& t = mLayout->T();
        assert(static_cast<const Array&>(*mLayout).IndexInBounds(index));
        const size_t elementSize = LayoutElement::GetNextBoundaryOffset(t.GetSizeInBytes());
        return { &t, mBytes, mOffset + elementSize * index };
    }

    ConstElementRef::Ptr ConstElementRef::operator&() noexcept(!IS_DEBUG)
    {
        return { *this };
    }

    bool ConstElementRef::Exists() const noexcept
    {
        return mLayout->Exists();
    }

    DCB_REF_CONST(ConstElementRef, Matrix);
    DCB_REF_CONST(ConstElementRef, Float4);
    DCB_REF_CONST(ConstElementRef, Float3);
    DCB_REF_CONST(ConstElementRef, Float2);
    DCB_REF_CONST(ConstElementRef, Float);
    DCB_REF_CONST(ConstElementRef, Bool);



    ElementRef::Ptr::Ptr(ElementRef& ref) noexcept
        : mRef(ref)
    {
    }

    DCB_PTR_CONVERSION(ElementRef, Matrix);
    DCB_PTR_CONVERSION(ElementRef, Float4);
    DCB_PTR_CONVERSION(ElementRef, Float3);
    DCB_PTR_CONVERSION(ElementRef, Float2);
    DCB_PTR_CONVERSION(ElementRef, Float);
    DCB_PTR_CONVERSION(ElementRef, Bool);

    ElementRef::ElementRef(const LayoutElement* pLayout, std::byte* pBytes, size_t offset) noexcept
        : mLayout(pLayout), mBytes(pBytes), mOffset(offset)
    {
    }

    ElementRef ElementRef::operator[](const std::wstring& key) noexcept(!IS_DEBUG)
    {
        return { &(*mLayout)[key], mBytes, mOffset };
    }

    ElementRef ElementRef::operator[](size_t index) noexcept(!IS_DEBUG)
    {
        const LayoutElement& t = mLayout->T();
        assert(static_cast<const Array&>(*mLayout).IndexInBounds(index));
        const size_t elementSize = LayoutElement::GetNextBoundaryOffset(t.GetSizeInBytes());
        return { &t, mBytes, mOffset + elementSize * index };
    }

    ElementRef::Ptr ElementRef::operator&() noexcept(!IS_DEBUG)
    {
        return { *this };
    }

    ElementRef::operator ConstElementRef() const noexcept
    {
        return { mLayout, mBytes, mOffset };
    }

    bool ElementRef::Exists() const noexcept
    {
        return mLayout->Exists();
    }

    DCB_REF_NONCONST(ElementRef, Matrix);
    DCB_REF_NONCONST(ElementRef, Float4);
    DCB_REF_NONCONST(ElementRef, Float3);
    DCB_REF_NONCONST(ElementRef, Float2);
    DCB_REF_NONCONST(ElementRef, Float);
    DCB_REF_NONCONST(ElementRef, Bool);


    

    Buffer Buffer::Make(RawLayout&& layout) noexcept(!IS_DEBUG)
    {
        return { LayoutCodex::Resolve(std::move(layout)) };
    }

    Buffer Buffer::Make(const CookedLayout& layout) noexcept(!IS_DEBUG)
    {
        return { layout.ShareRoot() };
    }

    ElementRef Buffer::operator[](const std::wstring& key) noexcept(!IS_DEBUG)
    {
        return { &(*mLayout)[key], mBytes.data(), 0u };
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

    const LayoutElement& Buffer::GetLayout() const noexcept
    {
        return *mLayout;
    }

    std::shared_ptr<LayoutElement> Buffer::ShareLayout() const noexcept
    {
        return mLayout;
    }

    Buffer::Buffer(const CookedLayout& layout) noexcept
        : mLayout(layout.ShareRoot()), mBytes(mLayout->GetOffsetEnd())
    {
    }

    // temporary
    bool ValidateSymbolName(const std::wstring& name) noexcept
    {
        return !name.empty() && !std::isdigit(name.front()) && std::all_of(name.begin(), name.end(), [](wchar_t c) { return std::isalnum(c) || c == L'_'; });
    }
}
