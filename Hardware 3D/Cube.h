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

	template<class V>
	static IndexedTriangleList<V> MakeIndependent()
	{
		constexpr float side = 0.5f;

		std::vector<V> vertices(24);
		vertices[0].Position = { -side, -side, -side };	// 0 near side
		vertices[1].Position = { side, -side, -side };	// 1
		vertices[2].Position = { -side, side, -side };	// 2
		vertices[3].Position = { side, side, -side };	// 3
		vertices[4].Position = { -side, -side, side };	// 4 far side
		vertices[5].Position = { side, -side, side };	// 5
		vertices[6].Position = { -side, side, side };	// 6
		vertices[7].Position = { side, side, side };	// 7
		vertices[8].Position = { -side, -side, -side };	// 8 left side
		vertices[9].Position = { -side, side, -side };	// 9
		vertices[10].Position = { -side, -side, side };	// 10
		vertices[11].Position = { -side, side, side };	// 11
		vertices[12].Position = { side, -side, -side };	// 12 right side
		vertices[13].Position = { side, side, -side };	// 13
		vertices[14].Position = { side, -side, side };	// 14
		vertices[15].Position = { side, side, side };	// 15
		vertices[16].Position = { -side, -side, -side };// 16 bottom side
		vertices[17].Position = { side, -side, -side };	// 17
		vertices[18].Position = { -side, -side, side };	// 18
		vertices[19].Position = { side, -side, side };	// 19
		vertices[20].Position = { -side, side, -side };	// 20 top side
		vertices[21].Position = { side, side, -side };	// 21
		vertices[22].Position = { -side, side, side };	// 22
		vertices[23].Position = { side, side, side };	// 23

		return{
			std::move(vertices),{
				0,2, 1,    2,3,1,
				4,5, 7,    4,7,6,
				8,10, 9,  10,11,9,
				12,13,15, 12,15,14,
				16,17,18, 18,17,19,
				20,23,21, 20,22,23
			}
		};
	}
};
