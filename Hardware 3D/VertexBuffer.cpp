#include "VertexBuffer.h"

void VertexBuffer::Bind(const Graphics& gfx) noexcept
{
	GetDeviceContext(gfx)->IASetVertexBuffers(0u, 1u, mBuffer.GetAddressOf(), &mStride, &mOffset);
}