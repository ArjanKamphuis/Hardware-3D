#include "DynamicConstantBuffer.h"

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
size_t eltype::Finalize(size_t offset) \
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

    LayoutElement& LayoutElement::operator[](const std::wstring& key)
    {
        assert(false && "Cannot access member on non Struct");
        return *this;
    }

    const LayoutElement& LayoutElement::operator[](const std::wstring& key) const
    {
        assert(false && "Cannot access member on non Struct");
        return *this;
    }

    LayoutElement& LayoutElement::T()
    {
        assert(false);
        return *this;
    }

    const LayoutElement& LayoutElement::T() const
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

    size_t LayoutElement::GetNextBoundaryOffset(size_t offset)
    {
        return offset + (16u - offset % 16) % 16;
    }

    class Empty : public LayoutElement
    {
    public:
        size_t GetOffsetEnd() const noexcept override final
        {
            return 0u;
        }
        bool Exists() const noexcept override final
        {
            return false;
        }
    protected:
        size_t Finalize(size_t offset) override final
        {
            return 0u;
        }
        size_t ComputeSize() const noexcept(!IS_DEBUG) override final
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



    LayoutElement& Struct::operator[](const std::wstring& key)
    {
        const auto it = mMap.find(key);
        return it == mMap.end() ? EmptyLayoutElement : *it->second;
    }

    const LayoutElement& Struct::operator[](const std::wstring& key) const
    {
        return const_cast<Struct&>(*this)[key];
    }

    size_t Struct::GetOffsetEnd() const noexcept
    {
        return LayoutElement::GetNextBoundaryOffset(mElements.back()->GetOffsetEnd());
    }

    size_t Struct::Finalize(size_t offset)
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
    


    LayoutElement& Array::T()
    {
        return *mElement;
    }

    const LayoutElement& Array::T() const
    {
        return *mElement;
    }

    size_t Array::GetOffsetEnd() const noexcept
    {
        return GetOffsetBegin() + LayoutElement::GetNextBoundaryOffset(mElement->GetSizeInBytes()) * mSize;
    }

    bool Array::IndexInBounds(size_t index) const noexcept
    {
        return index < mSize;
    }

    size_t Array::Finalize(size_t offset)
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



    Layout::Layout()
        : mLayout(std::make_shared<Struct>())
    {
    }

    Layout::Layout(std::shared_ptr<LayoutElement> pLayout)
        : mLayout(std::move(pLayout))
    {
    }

    LayoutElement& Layout::operator[](const std::wstring& key)
    {
        assert(!mFinalized && "Cannot modify finalized layout");
        return (*mLayout)[key];
    }

    size_t Layout::GetSizeInBytes() const noexcept
    {
        return mLayout->GetSizeInBytes();
    }

    std::shared_ptr<LayoutElement> Layout::Finalize()
    {
        mLayout->Finalize(0u);
        mFinalized = true;
        return mLayout;
    }



    ConstElementRef::Ptr::Ptr(ConstElementRef& ref)
        : mRef(ref)
    {
    }

    DCB_PTR_CONVERSION(ConstElementRef, Matrix, const);
    DCB_PTR_CONVERSION(ConstElementRef, Float4, const);
    DCB_PTR_CONVERSION(ConstElementRef, Float3, const);
    DCB_PTR_CONVERSION(ConstElementRef, Float2, const);
    DCB_PTR_CONVERSION(ConstElementRef, Float, const);
    DCB_PTR_CONVERSION(ConstElementRef, Bool, const);

    ConstElementRef::ConstElementRef(const LayoutElement* pLayout, std::byte* pBytes, size_t offset)
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



    ElementRef::Ptr::Ptr(ElementRef& ref)
        : mRef(ref)
    {
    }

    DCB_PTR_CONVERSION(ElementRef, Matrix);
    DCB_PTR_CONVERSION(ElementRef, Float4);
    DCB_PTR_CONVERSION(ElementRef, Float3);
    DCB_PTR_CONVERSION(ElementRef, Float2);
    DCB_PTR_CONVERSION(ElementRef, Float);
    DCB_PTR_CONVERSION(ElementRef, Bool);

    ElementRef::ElementRef(const LayoutElement* pLayout, std::byte* pBytes, size_t offset)
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



    Buffer::Buffer(Layout& layout)
        : mLayout(std::static_pointer_cast<Struct>(layout.Finalize())), mBytes(mLayout->GetOffsetEnd())
    {
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

    std::shared_ptr<LayoutElement> Buffer::CloneLayout() const
    {
        return mLayout;
    }
}
