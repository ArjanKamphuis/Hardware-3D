#pragma once

#include "IndexedTriangleList.h"

class Plane
{
public:
	static IndexedTriangleList MakeTesselatedTextured(Dvtx::VertexLayout layout, size_t divisionsX, size_t divisionsY);
	static IndexedTriangleList Make();
};
