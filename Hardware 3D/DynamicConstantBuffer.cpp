#include "DynamicConstantBuffer.h"

using namespace DirectX;

namespace Dcb
{
    LayoutElement::LayoutElement(size_t offset)
        : mOffset(offset)
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

    size_t LayoutElement::GetOffsetBegin() const noexcept
    {
        return mOffset;
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

    ElementRef::ElementRef(const LayoutElement* pLayout, std::byte* pBytes)
        : mLayout(pLayout), mBytes(pBytes)
    {
    }

    ElementRef ElementRef::operator[](const wchar_t* key) noexcept(!IS_DEBUG)
    {
        return { &(*mLayout)[key], mBytes };
    }

    Buffer::Buffer(const Struct& pLayout)
        : mLayout(&pLayout), mBytes(pLayout.GetOffsetEnd())
    {
    }

    ElementRef Buffer::operator[](const wchar_t* key) noexcept(!IS_DEBUG)
    {
        return { &(*mLayout)[key], mBytes.data() };
    }
}
