#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"

class VertexBuffer : public Bindable
{
public:
	template <class V>
	VertexBuffer(const Graphics& gfx, const std::vector<V>& vertices)
		: mStride(sizeof(V))
	{
		INFOMAN(gfx);

		D3D11_BUFFER_DESC vbd = {};
		vbd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vbd.ByteWidth = static_cast<UINT>(vertices.size() * sizeof(V));
		vbd.Usage = D3D11_USAGE_IMMUTABLE;
		D3D11_SUBRESOURCE_DATA vInitData = {};
		vInitData.pSysMem = vertices.data();
		GFX_THROW_INFO(GetDevice(gfx)->CreateBuffer(&vbd, &vInitData, &mBuffer));
	}
	VertexBuffer(const Graphics& gfx, const hw3dexp::VertexBuffer& vbuf)
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

	void Bind(const Graphics& gfx) noexcept override;

private:
	UINT mStride;
	UINT mOffset = 0u;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mBuffer;
};

