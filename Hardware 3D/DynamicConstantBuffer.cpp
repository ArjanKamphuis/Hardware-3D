#include "DynamicConstantBuffer.h"

using namespace DirectX;

namespace Dcb
{
    LayoutElement::LayoutElement(size_t offset)
        : mOffset(offset)
    {
    }

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

    ElementRef::ElementPtr::ElementPtr(ElementRef& ref)
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

    ElementRef::ElementPtr ElementRef::operator&() noexcept(!IS_DEBUG)
    {
        return { *this };
    }

    Buffer::Buffer(std::shared_ptr<Struct> pLayout)
        : mLayout(pLayout), mBytes(pLayout->GetOffsetEnd())
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
