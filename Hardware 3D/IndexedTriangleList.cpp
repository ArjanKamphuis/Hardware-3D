#include "IndexedTriangleList.h"

using namespace DirectX;
using namespace Dvtx;

IndexedTriangleList::IndexedTriangleList(VertexBuffer vertices, std::vector<unsigned short> indices)
	: Vertices(std::move(vertices)), Indices(std::move(indices))
{
	assert(Vertices.Size() > 2);
	assert(Indices.size() % 3 == 0);
}

void XM_CALLCONV IndexedTriangleList::Transform(FXMMATRIX transform)
{
	for (size_t i = 0; i < Vertices.Size(); i++)
	{
		XMFLOAT3& position = Vertices[i].Attr<VertexLayout::ElementType::Position3D>();
		XMStoreFloat3(&position, XMVector3Transform(XMLoadFloat3(&position), transform));
	}
}

void IndexedTriangleList::SetNormalsIndependentFlat() noexcept(!IS_DEBUG)
{
	using ElementType = VertexLayout::ElementType;
	assert(Indices.size() % 3 == 0 && Indices.size() > 0);

	for (size_t i = 0; i < Indices.size(); i += 3)
	{
		Vertex v0 = Vertices[Indices[i]];
		Vertex v1 = Vertices[Indices[i + 1]];
		Vertex v2 = Vertices[Indices[i + 2]];

		const XMVECTOR p0 = XMLoadFloat3(&v0.Attr<ElementType::Position3D>());
		const XMVECTOR p1 = XMLoadFloat3(&v1.Attr<ElementType::Position3D>());
		const XMVECTOR p2 = XMLoadFloat3(&v2.Attr<ElementType::Position3D>());

		const XMVECTOR n = XMVector3Normalize(XMVector3Cross((p1 - p0), (p2 - p0)));

		XMStoreFloat3(&v0.Attr<ElementType::Normal>(), n);
		XMStoreFloat3(&v1.Attr<ElementType::Normal>(), n);
		XMStoreFloat3(&v2.Attr<ElementType::Normal>(), n);
	}
}
