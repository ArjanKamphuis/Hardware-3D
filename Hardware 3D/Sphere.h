#pragma once

#include "IndexedTriangleList.h"

class Sphere
{
public:
	template<class V>
	static IndexedTriangleList<V> MakeTesselated(int latDiv, int longDiv)
	{
		assert(latDiv >= 3);
		assert(longDiv >= 3);

		constexpr float radius = 1.0f;
		const DirectX::XMVECTOR base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
		const float latitudeAngle = DirectX::XM_PI / latDiv;
		const float longitudeAngle = DirectX::XM_2PI / longDiv;

		std::vector<V> vertices;
		for (int iLat = 1; iLat < latDiv; iLat++)
		{
			const DirectX::XMVECTOR latBase = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationX(latitudeAngle * iLat));
			for (int iLong = 0; iLong < longDiv; iLong++)
			{
				vertices.emplace_back();
				const DirectX::XMVECTOR v = DirectX::XMVector3Transform(latBase, DirectX::XMMatrixRotationZ(longitudeAngle * iLong));
				DirectX::XMStoreFloat3(&vertices.back().Position, v);
			}
		}

		const unsigned short iNorthPole = static_cast<unsigned short>(vertices.size());
		vertices.emplace_back();
		DirectX::XMStoreFloat3(&vertices.back().Position, base);
		const unsigned short iSouthPole = static_cast<unsigned short>(vertices.size());
		vertices.emplace_back();
		DirectX::XMStoreFloat3(&vertices.back().Position, DirectX::XMVectorNegate(base));

		const auto calcIdx = [longDiv](unsigned short iLat, unsigned short iLong)
		{
			return iLat * longDiv + iLong;
		};

		std::vector<unsigned short> indices;
		for (unsigned short iLat = 0; iLat < latDiv - 2; iLat++)
		{
			for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
			{
				indices.push_back(calcIdx(iLat, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat, iLong + 1));
				indices.push_back(calcIdx(iLat + 1, iLong));
				indices.push_back(calcIdx(iLat + 1, iLong + 1));
			}

			indices.push_back(calcIdx(iLat, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat, 0));
			indices.push_back(calcIdx(iLat + 1, longDiv - 1));
			indices.push_back(calcIdx(iLat + 1, 0));
		}

		for (unsigned short iLong = 0; iLong < longDiv - 1; iLong++)
		{
			indices.push_back(iNorthPole);
			indices.push_back(calcIdx(0, iLong));
			indices.push_back(calcIdx(0, iLong + 1));

			indices.push_back(calcIdx(latDiv - 2, iLong + 1));
			indices.push_back(calcIdx(latDiv - 2, iLong));
			indices.push_back(iSouthPole);
		}

		indices.push_back(iNorthPole);
		indices.push_back(calcIdx(0, longDiv - 1));
		indices.push_back(0);

		indices.push_back(calcIdx(latDiv - 2, 0));
		indices.push_back(calcIdx(latDiv - 2, longDiv - 1));
		indices.push_back(iSouthPole);

		return { std::move(vertices), std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(12, 24);
	}
};
