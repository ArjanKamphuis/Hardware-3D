#pragma once

#include "Bindable.h"
#include "GraphicsThrowMacros.h"
#include "Vertex.h"

namespace Bind
{
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

		VertexBuffer(const Graphics& gfx, const Dvtx::VertexBuffer& vbuf);
		void Bind(const Graphics& gfx) noexcept override;

	private:
		UINT mStride;
		UINT mOffset = 0u;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mBuffer;
	};
}
