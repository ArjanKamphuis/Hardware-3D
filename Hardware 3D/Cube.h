#pragma once

#include "IndexedTriangleList.h"

class Cube
{
public:
	static IndexedTriangleList MakeIndependent(Dvtx::VertexLayout layout);
	static IndexedTriangleList MakeIndependentTextured();
};
