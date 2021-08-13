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

    size_t LayoutElement::ResolveFloat3() const noexcept(!IS_DEBUG)
    {
        assert(false && "Cannot resolve LayoutElement type");
        return 0;
    }

    size_t Float3::ResolveFloat3() const noexcept(!IS_DEBUG)
    {
        return GetOffsetBegin();
    }

    size_t Float3::GetOffsetEnd() const noexcept
    {
        return GetOffsetBegin() + sizeof(XMFLOAT3);
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

    ElementRef::operator DirectX::XMFLOAT3&() noexcept(!IS_DEBUG)
    {
        return *reinterpret_cast<XMFLOAT3*>(mBytes + mLayout->ResolveFloat3());
    }

    XMFLOAT3& ElementRef::operator=(const XMFLOAT3& rhs) noexcept(!IS_DEBUG)
    {
        return static_cast<XMFLOAT3&>(*this) = rhs;
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
