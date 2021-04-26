#include "AssTest.h"

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "IndexedTriangleList.h"

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
	struct Vertex
	{
		XMFLOAT3 Position;
		XMFLOAT3 Normal;
	};

	const std::vector<D3D11_INPUT_ELEMENT_DESC> ied =
	{
		{ "POSITION", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 0u, D3D11_INPUT_PER_VERTEX_DATA, 0u },
		{ "NORMAL", 0u, DXGI_FORMAT_R32G32B32_FLOAT, 0u, 12u, D3D11_INPUT_PER_VERTEX_DATA, 0u }
	};

	Assimp::Importer imp;
	const auto pModel = imp.ReadFile("Models/suzanne.obj", aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	const auto pMesh = pModel->mMeshes[0];

	IndexedTriangleList<Vertex> model;
	model.Vertices.reserve(static_cast<size_t>(pMesh->mNumVertices));
	model.Indices.reserve(static_cast<size_t>(pMesh->mNumFaces) * 3);

	for (UINT i = 0; i < pMesh->mNumVertices; i++)
		model.Vertices.push_back({ *reinterpret_cast<XMFLOAT3*>(&pMesh->mVertices[i]), *reinterpret_cast<XMFLOAT3*>(&pMesh->mNormals[i]) });

	for (UINT i = 0; i < pMesh->mNumFaces; i++)
	{
		const auto& face = pMesh->mFaces[i];
		assert(face.mNumIndices == 3);
		for (int j = 0; j < 3; j++)
			model.Indices.push_back(face.mIndices[j]);
	}

	AddRequiredStaticBindings(gfx, L"PhongVS.cso", L"PhongPS.cso", ied, model);
}
