#include "VertexBuffer.h"

namespace Bind
{
	VertexBuffer::VertexBuffer(const Graphics& gfx, const Dvtx::VertexBuffer& vbuf)
		: mStride(static_cast<UINT>(vbuf.GetLayout().Size()))
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC vbd = {};
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.ByteWidth = static_cast<UINT>(vbuf.SizeBytes());
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		D3D11_SUBRESOURCE_DATA vInitData = {};
		vInitData.pSysMem = vbuf.GetData();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&vbd, &vInitData, &mBuffer));
	}
	void VertexBuffer::Bind(const Graphics& gfx) noexcept
	{
		GetDeviceContext(gfx)->IASetVertexBuffers(0u, 1u, mBuffer.GetAddressOf(), &mStride, &mOffset);
	}
}