#pragma once

#include "IndexedTriangleList.h"

class Cone
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int longDiv)
	{
		assert(longDiv >= 3);

		const DirectX::XMVECTOR base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAngle = DirectX::XM_2PI / longDiv;

		// base vertices
		std::vector<V> vertices;
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			// near base
			vertices.emplace_back();
			DirectX::XMVECTOR v = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationZ(longitudeAngle * iLong));
			DirectX::XMStoreFloat3(&vertices.back().Position, v);
		}

		// center
		vertices.emplace_back();
		vertices.back().Position = { 0.0f, 0.0f, -1.0f };
		const unsigned short iCenter = static_cast<unsigned short>(vertices.size() - 1);

		// tip
		vertices.emplace_back();
		vertices.back().Position = { 0.0f, 0.0f, 1.0f };
		const unsigned short iTip = static_cast<unsigned short>(vertices.size() - 1);

		// base indices
		std::vector<unsigned short> indices;
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iCenter);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iLong);
		}

		// cone indices
		for (unsigned short iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iLong);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iTip);
		}

		return { std::move(vertices), std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(24);
	}
};
