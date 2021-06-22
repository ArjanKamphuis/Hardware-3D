#pragma once

#include "Vertex.h"

class IndexedTriangleList
{
public:
	IndexedTriangleList() = default;
	IndexedTriangleList(Dvtx::VertexBuffer vertices, std::vector<unsigned short> indices);

	void XM_CALLCONV Transform(DirectX::FXMMATRIX transform);

	void SetNormalsIndependentFlat() noexcept(!IS_DEBUG);

public:
	Dvtx::VertexBuffer Vertices;
	std::vector<unsigned short> Indices;
};
