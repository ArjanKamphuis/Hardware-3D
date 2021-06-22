#include "Cube.h"

using namespace Dvtx;
using ElementType = VertexLayout::ElementType;

IndexedTriangleList Cube::MakeIndependent(VertexLayout layout)
{
	constexpr float side = 0.5f;

	VertexBuffer vertices(std::move(layout), 24u);
	vertices[0].Attr<ElementType::Position3D>() = { -side, -side, -side };	// 0 near side
	vertices[1].Attr<ElementType::Position3D>() = { side, -side, -side };	// 1
	vertices[2].Attr<ElementType::Position3D>() = { -side, side, -side };	// 2
	vertices[3].Attr<ElementType::Position3D>() = { side, side, -side };	// 3
	vertices[4].Attr<ElementType::Position3D>() = { -side, -side, side };	// 4 far side
	vertices[5].Attr<ElementType::Position3D>() = { side, -side, side };	// 5
	vertices[6].Attr<ElementType::Position3D>() = { -side, side, side };	// 6
	vertices[7].Attr<ElementType::Position3D>() = { side, side, side };	// 7
	vertices[8].Attr<ElementType::Position3D>() = { -side, -side, -side };	// 8 left side
	vertices[9].Attr<ElementType::Position3D>() = { -side, side, -side };	// 9
	vertices[10].Attr<ElementType::Position3D>() = { -side, -side, side };	// 10
	vertices[11].Attr<ElementType::Position3D>() = { -side, side, side };	// 11
	vertices[12].Attr<ElementType::Position3D>() = { side, -side, -side };	// 12 right side
	vertices[13].Attr<ElementType::Position3D>() = { side, side, -side };	// 13
	vertices[14].Attr<ElementType::Position3D>() = { side, -side, side };	// 14
	vertices[15].Attr<ElementType::Position3D>() = { side, side, side };	// 15
	vertices[16].Attr<ElementType::Position3D>() = { -side, -side, -side };// 16 bottom side
	vertices[17].Attr<ElementType::Position3D>() = { side, -side, -side };	// 17
	vertices[18].Attr<ElementType::Position3D>() = { -side, -side, side };	// 18
	vertices[19].Attr<ElementType::Position3D>() = { side, -side, side };	// 19
	vertices[20].Attr<ElementType::Position3D>() = { -side, side, -side };	// 20 top side
	vertices[21].Attr<ElementType::Position3D>() = { side, side, -side };	// 21
	vertices[22].Attr<ElementType::Position3D>() = { -side, side, side };	// 22
	vertices[23].Attr<ElementType::Position3D>() = { side, side, side };	// 23

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

IndexedTriangleList Cube::MakeIndependentTextured()
{
	IndexedTriangleList itl = MakeIndependent(std::move(VertexLayout{}
		.Append(ElementType::Position3D)
		.Append(ElementType::Normal)
		.Append(ElementType::Texture2D)
	));

	itl.Vertices[0].Attr<ElementType::Texture2D>() = { 0.0f, 0.0f };
	itl.Vertices[1].Attr<ElementType::Texture2D>() = { 1.0f, 0.0f };
	itl.Vertices[2].Attr<ElementType::Texture2D>() = { 0.0f, 1.0f };
	itl.Vertices[3].Attr<ElementType::Texture2D>() = { 1.0f, 1.0f };
	itl.Vertices[4].Attr<ElementType::Texture2D>() = { 0.0f, 0.0f };
	itl.Vertices[5].Attr<ElementType::Texture2D>() = { 1.0f, 0.0f };
	itl.Vertices[6].Attr<ElementType::Texture2D>() = { 0.0f, 1.0f };
	itl.Vertices[7].Attr<ElementType::Texture2D>() = { 1.0f, 1.0f };
	itl.Vertices[8].Attr<ElementType::Texture2D>() = { 0.0f, 0.0f };
	itl.Vertices[9].Attr<ElementType::Texture2D>() = { 1.0f, 0.0f };
	itl.Vertices[10].Attr<ElementType::Texture2D>() = { 0.0f, 1.0f };
	itl.Vertices[11].Attr<ElementType::Texture2D>() = { 1.0f, 1.0f };
	itl.Vertices[12].Attr<ElementType::Texture2D>() = { 0.0f, 0.0f };
	itl.Vertices[13].Attr<ElementType::Texture2D>() = { 1.0f, 0.0f };
	itl.Vertices[14].Attr<ElementType::Texture2D>() = { 0.0f, 1.0f };
	itl.Vertices[15].Attr<ElementType::Texture2D>() = { 1.0f, 1.0f };
	itl.Vertices[16].Attr<ElementType::Texture2D>() = { 0.0f, 0.0f };
	itl.Vertices[17].Attr<ElementType::Texture2D>() = { 1.0f, 0.0f };
	itl.Vertices[18].Attr<ElementType::Texture2D>() = { 0.0f, 1.0f };
	itl.Vertices[19].Attr<ElementType::Texture2D>() = { 1.0f, 1.0f };
	itl.Vertices[20].Attr<ElementType::Texture2D>() = { 0.0f, 0.0f };
	itl.Vertices[21].Attr<ElementType::Texture2D>() = { 1.0f, 0.0f };
	itl.Vertices[22].Attr<ElementType::Texture2D>() = { 0.0f, 1.0f };
	itl.Vertices[23].Attr<ElementType::Texture2D>() = { 1.0f, 1.0f };

	return itl;
}
