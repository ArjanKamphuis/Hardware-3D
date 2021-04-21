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
		const USHORT iCenterNear = static_cast<USHORT>(vertices.size() - 1);

		// far center
		vertices.emplace_back();
		vertices.back().Position = { 0.0f, 0.0f, 1.0f };
		const USHORT iCenterFar = static_cast<USHORT>(vertices.size() - 1);

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
		std::vector<USHORT> indices;
		for (USHORT iLong = 0; iLong < longDiv; iLong++)
		{
			const USHORT i = iLong * 2u;
			const USHORT mod = longDiv * 2u;
			indices.push_back(i + 2);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back(i + 3);
			indices.push_back((i + 2) % mod + 2);
			indices.push_back((i + 3) % mod + 2);
			indices.push_back(i + 3);
		}

		// base indices
		for (USHORT iLong = 0; iLong < longDiv; iLong++)
		{
			const USHORT i = iLong * 2u;
			const USHORT mod = longDiv * 2u;
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
	static IndexedTriangleList<V> MakeTesselatedIndependentCapNormals(int longDiv)
	{
		assert(longDiv >= 3);

		const DirectX::XMVECTOR base = DirectX::XMVectorSet(1.0f, 0.0f, -1.0f, 0.0f);
		const DirectX::XMVECTOR offset = DirectX::XMVectorSet(0.0f, 0.0f, 2.0f, 0.0f);
		const float longitudeAngle = DirectX::XM_2PI / longDiv;

		std::vector<V> vertices;
		std::vector<USHORT> indices;

		// near center
		const USHORT iCenterNear = static_cast<USHORT>(vertices.size());
		vertices.emplace_back();
		vertices.back().Position = { 0.0f, 0.0f, -1.0f };
		vertices.back().Normal = { 0.0f, 0.0f, -1.0f };

		// near base
		const USHORT iBaseNear = static_cast<USHORT>(vertices.size());
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			DirectX::XMVECTOR v = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationZ(longitudeAngle * iLong));
			DirectX::XMStoreFloat3(&vertices.back().Position, v);
			vertices.back().Normal = { 0.0f, 0.0f, -1.0f };
		}

		// far center
		const USHORT iCenterFar = static_cast<USHORT>(vertices.size());
		vertices.emplace_back();
		vertices.back().Position = { 0.0f, 0.0f, 1.0f };
		vertices.back().Normal = { 0.0f, 0.0f, 1.0f };

		// far base
		const USHORT iBaseFar = static_cast<USHORT>(vertices.size());
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			vertices.emplace_back();
			DirectX::XMVECTOR v = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationZ(longitudeAngle * iLong));
			DirectX::XMStoreFloat3(&vertices.back().Position, DirectX::XMVectorAdd(v, offset));
			vertices.back().Normal = { 0.0f, 0.0f, 1.0f };
		}

		// fusilage
		const USHORT iFusilage = static_cast<USHORT>(vertices.size());
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			// near base
			vertices.emplace_back();
			DirectX::XMVECTOR v = DirectX::XMVector3Transform(base, DirectX::XMMatrixRotationZ(longitudeAngle * iLong));
			DirectX::XMStoreFloat3(&vertices.back().Position, v);
			vertices.back().Normal = { vertices.back().Position.x, vertices.back().Position.y, 0.0f };

			// far base
			vertices.emplace_back();
			DirectX::XMStoreFloat3(&vertices.back().Position, DirectX::XMVectorAdd(v, offset));
			vertices.back().Normal = { vertices.back().Position.x, vertices.back().Position.y, 0.0f };
		}

		// near base indices
		for (USHORT iLong = 0; iLong < longDiv; iLong++)
		{
			const USHORT i = iLong;
			const USHORT mod = longDiv;
			indices.push_back(i + iBaseNear);
			indices.push_back(iCenterNear);
			indices.push_back((i + 1) % mod + iBaseNear);
		}

		// far base indices
		for (USHORT iLong = 0; iLong < longDiv; iLong++)
		{
			const USHORT i = iLong;
			const USHORT mod = longDiv;
			indices.push_back(iCenterFar);
			indices.push_back(i + iBaseFar);
			indices.push_back((i + 1) % mod + iBaseFar);
		}

		// fusilage indices
		for (USHORT iLong = 0; iLong < longDiv; iLong++)
		{
			const USHORT i = iLong * 2u;
			const USHORT mod = longDiv * 2u;
			indices.push_back(i + iFusilage);
			indices.push_back((i + 2) % mod + iFusilage);
			indices.push_back(i + 1 + iFusilage);
			indices.push_back((i + 2) % mod + iFusilage);
			indices.push_back((i + 3) % mod + iFusilage);
			indices.push_back(i + 1 + iFusilage);
		}

		return { std::move(vertices), std::move(indices) };
	}

	template<class V>
	static IndexedTriangleList<V> Make()
	{
		return MakeTesselated<V>(24);
	}
};
