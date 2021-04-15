#pragma once

#include <DirectXMath.h>
#include <vector>

template<class V>
class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(std::vector<V> vertices, std::vector<unsigned short> indices)
		: Vertices(std::move(vertices)), Indices(std::move(indices))
	{
		assert(Vertices.size() > 2);
		assert(Indices.size() % 3 == 0);
	}

	void Transform(DirectX::FXMMATRIX transform)
	{
		for (V& v : Vertices)
		{
			const DirectX::XMVECTOR position = DirectX::XMLoadFloat3(&v.Position);
			DirectX::XMStoreFloat3(&v.Position, DirectX::XMVector3Transform(position, transform));
		}
	}

	void SetNormalsIndependentFlat() noexcept(!IS_DEBUG)
	{
		using namespace DirectX;
		assert(Indices.size() % 3 == 0 && Indices.size() > 0);

		for (size_t i = 0; i < Indices.size(); i += 3)
		{
			V& v0 = Vertices[Indices[i]];
			V& v1 = Vertices[Indices[i + 1]];
			V& v2 = Vertices[Indices[i + 2]];

			const XMVECTOR p0 = XMLoadFloat3(&v0.Position);
			const XMVECTOR p1 = XMLoadFloat3(&v1.Position);
			const XMVECTOR p2 = XMLoadFloat3(&v2.Position);

			const XMVECTOR n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

			XMStoreFloat3(&v0.Normal, n);
			XMStoreFloat3(&v1.Normal, n);
			XMStoreFloat3(&v2.Normal, n);
		}
	}

public:
	std::vector<V> Vertices;
	std::vector<unsigned short> Indices;
};
