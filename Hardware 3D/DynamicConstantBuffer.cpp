#include "DynamicConstantBuffer.h"

using namespace DirectX;

namespace Dcb
{
    LayoutElement::~LayoutElement()
    {
    }

    LayoutElement& LayoutElement::operator[](const wchar_t* key)
    {
        assert(false && "Cannot access member on non Struct");
        return *this;
    }

    const LayoutElement& LayoutElement::operator[](const wchar_t* key) const
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

    size_t LayoutElement::GetOffsetBegin() const noexcept
    {
        return mOffset;
    }

    size_t LayoutElement::GetSizeInBytes() const noexcept
    {
        return GetOffsetEnd() - GetOffsetBegin();
    }

    LayoutElement& Struct::operator[](const wchar_t* key)
    {
        return *mMap.at(key);
    }

    const LayoutElement& Struct::operator[](const wchar_t* key) const
    {
        return *mMap.at(key);
    }

    size_t Struct::GetOffsetEnd() const noexcept
    {
        return mElements.empty() ? GetOffsetBegin() : mElements.back()->GetOffsetEnd();
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
        assert(mElement);
        return GetOffsetBegin() + mElement->GetSizeInBytes() * mSize;
    }

    size_t Array::Finalize(size_t offset)
    {
        assert(mSize != 0u && mElement);
        mOffset = offset;
        mElement->Finalize(offset);
        return offset + mElement->GetSizeInBytes() * mSize;
    }

    Layout::Layout()
        : mLayout(std::make_shared<Struct>())
    {
    }

    LayoutElement& Layout::operator[](const wchar_t* key)
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

    ElementRef::Ptr::Ptr(ElementRef& ref)
        : mRef(ref)
    {
    }

    ElementRef::ElementRef(const LayoutElement* pLayout, std::byte* pBytes, size_t offset)
        : mLayout(pLayout), mBytes(pBytes), mOffset(offset)
    {
    }

    ElementRef ElementRef::operator[](const wchar_t* key) noexcept(!IS_DEBUG)
    {
        return { &(*mLayout)[key], mBytes, mOffset };
    }

    ElementRef ElementRef::operator[](size_t index) noexcept(!IS_DEBUG)
    {
        const LayoutElement& t = mLayout->T();
        return { &t, mBytes, mOffset + t.GetSizeInBytes() * index };
    }

    ElementRef::Ptr ElementRef::operator&() noexcept(!IS_DEBUG)
    {
        return { *this };
    }

    Buffer::Buffer(Layout& layout)
        : mLayout(std::static_pointer_cast<Struct>(layout.Finalize())), mBytes(mLayout->GetOffsetEnd())
    {
    }

    ElementRef Buffer::operator[](const wchar_t* key) noexcept(!IS_DEBUG)
    {
        return { &(*mLayout)[key], mBytes.data(), 0u };
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
