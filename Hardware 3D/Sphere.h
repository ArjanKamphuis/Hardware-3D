#pragma once

#include <optional>
#include "IndexedTriangleList.h"

class Sphere
{
public:
	static IndexedTriangleList MakeTesselated(Dvtx::VertexLayout layout, int latDiv, int longDiv);
	static IndexedTriangleList Make(std::optional<Dvtx::VertexLayout> layout = std::nullopt);
};
