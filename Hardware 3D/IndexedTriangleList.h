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

public:
	std::vector<V> Vertices;
	std::vector<unsigned short> Indices;
};
