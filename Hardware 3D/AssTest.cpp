#include "AssTest.h"

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "Vertex.h"

using namespace DirectX;

AssTest::AssTest(const Graphics& gfx, std::mt19937& rng, std::uniform_real_distribution<float>& adist, std::uniform_real_distribution<float>& ddist,
	std::uniform_real_distribution<float>& odist, std::uniform_real_distribution<float>& rdist, std::uniform_real_distribution<float>& scaledist, const DirectX::XMFLOAT3& color)
	: TestObject(gfx, rng, adist, ddist, odist, rdist), mMaterial({ color }), mScale(scaledist(rng))
{
	if (IsStaticInitialized())
		SetIndexFromStatic();
	else
		StaticInitialize(gfx);

	AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	AddBind(std::make_unique<PixelConstantBuffer<Material>>(gfx, mMaterial));
}

XMMATRIX AssTest::GetTransformMatrix() const noexcept
{
	return XMMatrixScaling(mScale, mScale, mScale) * TestObject::GetTransformMatrix();
}

void AssTest::StaticInitialize(const Graphics& gfx)
{
	using hw3dexp::VertexLayout;
	using ElementType = hw3dexp::VertexLayout::ElementType;

	hw3dexp::VertexBuffer vbuf(std::move(VertexLayout{}
		.Append(ElementType::Position3D)
		.Append(ElementType::Normal)
	));

	Assimp::Importer imp;
	const auto pModel = imp.ReadFile("Models/suzanne.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	const auto pMesh = pModel->mMeshes[0];

	for (UINT i = 0; i < pMesh->mNumVertices; i++)
		vbuf.EmplaceBack(*reinterpret_cast<XMFLOAT3*>(&pMesh->mVertices[i]), *reinterpret_cast<XMFLOAT3*>(&pMesh->mNormals[i]));

	std::vector<USHORT> indices;
	indices.reserve(static_cast<size_t>(pMesh->mNumFaces) * 3);
	for (UINT i = 0; i < pMesh->mNumFaces; i++)
	{
		const auto& face = pMesh->mFaces[i];
		assert(face.mNumIndices == 3);
		for (int j = 0; j < 3; j++)
			indices.push_back(face.mIndices[j]);
	}

	AddRequiredStaticBindings(gfx, L"PhongVS.cso", L"PhongPS.cso", vbuf.GetLayout().GetD3DLayout(), vbuf, indices);
}
