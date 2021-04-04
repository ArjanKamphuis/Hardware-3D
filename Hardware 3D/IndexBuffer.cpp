#include "IndexBuffer.h"

#include "GraphicsThrowMacros.h"

IndexBuffer::IndexBuffer(const Graphics& gfx, const std::vector<USHORT>& indices)
	: mSize(static_cast<UINT>(indices.size()))
{
	INFOMAN(gfx);

	D3D11_BUFFER_DESC ibd = {};
	ibd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibd.ByteWidth = static_cast<UINT>(mSize * sizeof(USHORT));
	ibd.Usage = D3D11_USAGE_IMMUTABLE;
	D3D11_SUBRESOURCE_DATA iInitData = {};
	iInitData.pSysMem = indices.data();
	GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&ibd, &iInitData, &mBuffer));
}

void IndexBuffer::Bind(const Graphics& gfx) noexcept
{
	GetDeviceContext(gfx)->IASetIndexBuffer(mBuffer.Get(), DXGI_FORMAT_R16_UINT, 0u);
}

UINT IndexBuffer::GetSize() const noexcept
{
	return mSize;
}
