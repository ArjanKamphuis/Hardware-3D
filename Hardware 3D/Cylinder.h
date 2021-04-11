#pragma once

#include "IndexedTriangleList.h"

class Cylinder
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int longDiv)
	{
		assert(longDiv >= 3);

		const DirectX::XMVECTOR base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const DirectX::XMVECTOR offset= DirectX::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
		const float longitudeAngle = DirectX::XM_2PI / longDiv;

		// near center
		std::vector<V> vertices;
		vertices.emplace_back();
		vertices.back().Position = { 0.0f, 0.0f, -1.0f };
		const unsigned short iCenterNear = static_cast<unsigned short>(vertices.size() - 1);

		// far center
		vertices.emplace_back();
		vertices.back().Position = { 0.0f, 0.0f, 1.0f };
		const unsigned short iCenterFar = static_cast<unsigned short>(vertices.size() - 1);

		// base vertices
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			// near base
			vertices.emplace_back();
			DirectX::XMVECTOR v = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationZ(longitudeAngle * iLong));
			DirectX::XMStoreFloat3(&vertices.back().Position, v);

			// far base
			vertices.emplace_back();
			DirectX::XMStoreFloat3(&vertices.back().Position, DirectX::XMVectorAdd(v, offset));
		}

		// side indices
		std::vector<unsigned short> indices;
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			const unsigned short i = iLong * 2u;
			const unsigned short mod = longDiv * 2u;
			indices.push_back(i + 2);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back(i + 3);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back((i + 3) % mod + 2);
			indices.push_back(i + 3);
		}

		// base indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			const unsigned short i = iLong * 2u;
			const unsigned short mod = longDiv * 2u;
			indices.push_back(i + 2);
			indices.push_back(iCenterNear);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back(iCenterFar);
			indices.push_back(i + 3);
			indices.push_back((i + 3) % mod + 2);
		}

		return { std::move(vertices), std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(24);
	}
};