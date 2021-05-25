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
