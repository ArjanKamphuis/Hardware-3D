#pragma once

#include "IndexedTriangleList.h"

class Cube
{
public:
	template<class V>
	static IndexedTriangleList<V> Make()
	{
		constexpr float side = 0.5f;

		std::vector<V> vertices(8);
		vertices[0].Position = { -side, -side, -side };
		vertices[1].Position = { side, -side, -side };
		vertices[2].Position = { -side, side, -side };
		vertices[3].Position = { side, side, -side };
		vertices[4].Position = { -side, -side, side };
		vertices[5].Position = { side, -side, side };
		vertices[6].Position = { -side, side, side };
		vertices[7].Position = { side, side, side };

		return { std::move(vertices), {
			0, 2, 1,	2, 3, 1,
			1, 3, 5,	3, 7, 5,
			2, 6, 3,	3, 6, 7,
			4, 5, 7,	4, 7, 6,
			0, 4, 2,	2, 4, 6,
			0, 1, 4,	1, 5, 4
		} };
	}

	template<class V>
	static IndexedTriangleList<V> MakeSkinned()
	{
		constexpr float side = 0.5f;

		std::vector<V> vertices(14);
		vertices[0].Position = { -side, -side, -side };
		vertices[0].TexCoord = { 2.0f / 3.0f, 0.0f };
		vertices[1].Position = { side, -side, -side };
		vertices[1].TexCoord = { 1.0f / 3.0f, 0.0f };
		vertices[2].Position = { -side, side, -side };
		vertices[2].TexCoord = { 2.0f / 3.0f, 0.25f };
		vertices[3].Position = { side, side, -side };
		vertices[3].TexCoord = { 1.0f / 3.0f, 0.25f };
		vertices[4].Position = { -side, -side, side };
		vertices[4].TexCoord = { 2.0f / 3.0f, 0.75f };
		vertices[5].Position = { side, -side, side };
		vertices[5].TexCoord = { 1.0f / 3.0f, 0.75f };
		vertices[6].Position = { -side, side, side };
		vertices[6].TexCoord = { 2.0f / 3.0f, 0.5f };
		vertices[7].Position = { side, side, side };
		vertices[7].TexCoord = { 1.0f / 3.0f, 0.5f };
		vertices[8].Position = { -side, -side, -side };
		vertices[8].TexCoord = { 2.0f / 3.0f, 1.0f };
		vertices[9].Position = { side, -side, -side };
		vertices[9].TexCoord = { 1.0f / 3.0f, 1.0f };
		vertices[10].Position = { -side, -side, -side };
		vertices[10].TexCoord = { 3.0f / 3.0f, 0.25f };
		vertices[11].Position = { -side, -side, side };
		vertices[11].TexCoord = { 3.0f / 3.0f, 0.5f };
		vertices[12].Position = { side, -side, -side };
		vertices[12].TexCoord = { 0.0f / 3.0f, 0.25f };
		vertices[13].Position = { side, -side, side };
		vertices[13].TexCoord = { 0.0f / 3.0f, 0.5f };

		return { std::move(vertices),{
			0,2,1,   2,3,1,
			4,8,5,   5,8,9,
			2,6,3,   3,6,7,
			4,5,7,   4,7,6,
			2,10,11, 2,11,6,
			12,3,7,  12,7,13
		} };
	}
};
