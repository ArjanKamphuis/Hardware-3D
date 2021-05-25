#include "Sphere.h"

using namespace DirectX;
using namespace Dvtx;

IndexedTriangleList Sphere::MakeTesselated(VertexLayout layout, int latDiv, int longDiv)
{
	assert(latDiv >= 3);
	assert(longDiv >= 3);

	constexpr float radius = 1.0f;
	const XMVECTOR base = DirectX::XMVectorSet(0.0f, 0.0f, radius, 0.0f);
	const float latitudeAngle = DirectX::XM_PI / latDiv;
	const float longitudeAngle = DirectX::XM_2PI / longDiv;

	VertexBuffer vb{ std::move(layout) };
	for (int iLat = 1; iLat < latDiv; iLat++)
	{
		const XMVECTOR latBase = XMVector3Transform(base, XMMatrixRotationX(latitudeAngle * iLat));
		for (int iLong = 0; iLong < longDiv; iLong++)
		{
			XMFLOAT3 calculatedPosition;
			const XMVECTOR v = XMVector3Transform(latBase, XMMatrixRotationZ(longitudeAngle * iLong));
			XMStoreFloat3(&calculatedPosition, v);
			vb.EmplaceBack(calculatedPosition);
		}
	}

	const unsigned short iNorthPole = static_cast<unsigned short>(vb.Size());
	XMFLOAT3 northPos;
	XMStoreFloat3(&northPos, base);
	vb.EmplaceBack(northPos);

	const unsigned short iSouthPole = static_cast<unsigned short>(vb.Size());
	XMFLOAT3 southPos;
	XMStoreFloat3(&southPos, DirectX::XMVectorNegate(base));
	vb.EmplaceBack(southPos);

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

	return { std::move(vb), std::move(indices) };
}

IndexedTriangleList Sphere::Make(std::optional<VertexLayout> layout)
{
	if (!layout)
		layout = VertexLayout{}.Append(VertexLayout::ElementType::Position3D);
	return MakeTesselated(std::move(*layout), 12, 24);
}
