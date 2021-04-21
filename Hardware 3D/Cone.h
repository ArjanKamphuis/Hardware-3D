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
		const USHORT iCenter = static_cast<USHORT>(vertices.size() - 1);

		// tip
		vertices.emplace_back();
		vertices.back().Position = { 0.0f, 0.0f, 1.0f };
		const USHORT iTip = static_cast<USHORT>(vertices.size() - 1);

		// base indices
		std::vector<USHORT> indices;
		for (USHORT iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iCenter);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iLong);
		}

		// cone indices
		for (USHORT iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iLong);
			indices.push_back((iLong + 1) % longDiv);
			indices.push_back(iTip);
		}

		return { std::move(vertices), std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> MakeTesselatedIndependentFaces(int longDiv)
	{
		assert(longDiv >= 3);

		const DirectX::XMVECTOR base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const float longitudeAngle = DirectX::XM_2PI / longDiv;

		std::vector<V> vertices;
		std::vector<USHORT> indices;

		// cone vertices
		const USHORT iCone = static_cast<USHORT>(vertices.size());
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			const float thetas[] = { longitudeAngle * iLong, longitudeAngle * (((iLong + 1) == longDiv) ? 0 : (iLong + 1)) };

			vertices.emplace_back();
			vertices.back().Position = { 0.0f, 0.0f, 1.0f };
			for (float theta : thetas)
			{
				vertices.emplace_back();
				const DirectX::XMVECTOR v = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationZ(theta));
				DirectX::XMStoreFloat3(&vertices.back().Position, v);
			}
		}

		// base vertices
		const USHORT iBaseCenter = static_cast<USHORT>(vertices.size());
		vertices.emplace_back();
		vertices.back().Position = { 0.0f, 0.0f, -1.0f };
		const USHORT iBaseEdge = static_cast<USHORT>(vertices.size());
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			DirectX::XMVECTOR v = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationZ(longitudeAngle * iLong));
			DirectX::XMStoreFloat3(&vertices.back().Position, v);
		}

		// cone indices
		for (USHORT i = 0; i < longDiv * 3u; i++)
			indices.push_back(i + iCone);

		// base indices
		for (USHORT iLong = 0; iLong < longDiv; iLong++)
		{
			indices.push_back(iBaseCenter);
			indices.push_back((iLong + 1) % longDiv + iBaseEdge);
			indices.push_back(iLong + iBaseEdge);
		}

		return { std::move(vertices), std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(24);
	}
};
