#include "Plane.h"

#include <array>

using namespace DirectX;
using namespace Dvtx;

IndexedTriangleList Plane::MakeTesselatedTextured(VertexLayout layout, size_t divisionsX, size_t divisionsY)
{
	assert(divisionsX >= 1);
	assert(divisionsY >= 1);

	constexpr float width = 1.0f;
	constexpr float height = 1.0f;
	const size_t nVerticesX = divisionsX + 1;
	const size_t nVerticesY = divisionsY + 1;
	VertexBuffer vb{ std::move(layout) };

	const float sideX = 0.5f * width;
	const float sideY = 0.5f * height;
	const float fdivx = static_cast<float>(divisionsX);
	const float fdivy = static_cast<float>(divisionsY);
	const float divisionSizeX = width / fdivx;
	const float divisionSizeY = height / fdivy;
	const float divisionSizeXtc = 1.0f / fdivx;
	const float divisionSizeYtc = 1.0f / fdivy;

	for (size_t y = 0; y < nVerticesY; y++)
	{
		const float fy = static_cast<float>(y);
		const float yPos = fy * divisionSizeY - sideY;
		const float yPosTc = 1.0f - fy * divisionSizeYtc;
		for (size_t x = 0; x < nVerticesX; x++)
		{
			const float fx = static_cast<float>(x);
			const float xPos = fx * divisionSizeX - sideX;
			const float xPosTc = fx * divisionSizeXtc;
			vb.EmplaceBack(XMFLOAT3{ xPos, yPos, 0.0f }, XMFLOAT3{ 0.0f, 0.0f, -1.0f }, XMFLOAT2{ xPosTc, yPosTc });
		}
	}

	std::vector<USHORT> indices;
	indices.reserve(static_cast<size_t>(divisionsX * divisionsX * divisionsY * divisionsY * 6));

	const auto vxy2i = [nVerticesX](size_t x, size_t y)
	{
		return static_cast<unsigned short>(y * nVerticesX + x);
	};

	for (size_t y = 0; y < divisionsY; y++)
	{
		for (size_t x = 0; x < divisionsX; x++)
		{
			const std::array<unsigned short, 4> indexArray = { vxy2i(x, y), vxy2i(x + 1, y), vxy2i(x,y + 1), vxy2i(x + 1, y + 1) };
			indices.push_back(indexArray[0]);
			indices.push_back(indexArray[2]);
			indices.push_back(indexArray[1]);
			indices.push_back(indexArray[1]);
			indices.push_back(indexArray[2]);
			indices.push_back(indexArray[3]);
		}
	}

	return { std::move(vb), std::move(indices) };
}

IndexedTriangleList Plane::Make()
{
	using ElementType = Dvtx::VertexLayout::ElementType;
	return MakeTesselatedTextured(std::move(Dvtx::VertexLayout{}.
		Append(ElementType::Position3D).
		Append(ElementType::Normal).
		Append(ElementType::Texture2D)
	), 1, 1);
}
