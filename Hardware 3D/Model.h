#pragma once

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "BindableBase.h"
#include "DrawableBase.h"
#include "Vertex.h"

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh(const Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs)
	{
		if (!IsStaticInitialized())
			StaticInitialize(gfx);

		for (auto& pb : bindPtrs)
		{
			if (auto pi = dynamic_cast<IndexBuffer*>(pb.get()))
			{
				AddIndexBuffer(std::unique_ptr<IndexBuffer>{ pi });
				pb.release();
			}
			else
				AddBind(std::move(pb));
		}

		AddBind(std::make_unique<TransformCBuf>(gfx, *this));
	}
	void Update(float dt) noexcept override
	{

	}
	void XM_CALLCONV Draw(const Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
	{
		DirectX::XMStoreFloat4x4(&mTransform, accumulatedTransform);
		Drawable::Draw(gfx);
	}
	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override
	{
		return DirectX::XMLoadFloat4x4(&mTransform);
	}

private:
	void StaticInitialize(const Graphics& gfx) override
	{
		AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	}

private:
	mutable DirectX::XMFLOAT4X4 mTransform = {};
};

class Node
{
	friend class Model;

public:
	Node(std::vector<Mesh*> meshPtrs, DirectX::CXMMATRIX transform) noexcept(!IS_DEBUG)
		: mMeshPtrs(std::move(meshPtrs))
	{
		DirectX::XMStoreFloat4x4(&mTransform, transform);
	}
	void XM_CALLCONV Draw(const Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
	{
		const DirectX::XMMATRIX built = DirectX::XMLoadFloat4x4(&mTransform) * accumulatedTransform;

		for (const Mesh* const pm : mMeshPtrs)
			pm->Draw(gfx, built);
		for (const auto& pc : mChildPtrs)
			pc->Draw(gfx, built);
	}

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
	{
		assert(pChild);
		mChildPtrs.emplace_back(std::move(pChild));
	}

private:
	std::vector<std::unique_ptr<Node>> mChildPtrs;
	std::vector<Mesh*> mMeshPtrs;
	DirectX::XMFLOAT4X4 mTransform;
};

class Model
{
public:
	Model(const Graphics& gfx, std::string filename)
	{
		Assimp::Importer imp;
		const auto pScene = imp.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

		for (size_t i = 0; i < pScene->mNumMeshes; i++)
			mMeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));

		mRoot = ParseNode(*pScene->mRootNode);
	}
	void XM_CALLCONV Draw(const Graphics& gfx, DirectX::FXMMATRIX transform) const
	{
		mRoot->Draw(gfx, transform);
	}

private:
	static std::unique_ptr<Mesh> ParseMesh(const Graphics& gfx, const aiMesh& mesh)
	{
		using hw3dexp::VertexLayout;
		using ElementType = VertexLayout::ElementType;

		hw3dexp::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(ElementType::Position3D)
			.Append(ElementType::Normal)
		));

		for (UINT i = 0; i < mesh.mNumVertices; i++)
			vbuf.EmplaceBack(*reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mVertices[i]), *reinterpret_cast<DirectX::XMFLOAT3*>(&mesh.mNormals[i]));

		std::vector<USHORT> indices;
		indices.reserve(static_cast<size_t>(mesh.mNumFaces * 3u));
		for (UINT i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			for (int j = 0; j < 3; j++)
				indices.push_back(face.mIndices[j]);
		}

		std::vector<std::unique_ptr<Bindable>> bindablePtrs;
		std::unique_ptr<VertexBuffer> pVbuf = std::make_unique<VertexBuffer>(gfx, vbuf);
		std::unique_ptr<VertexShader> pVS = std::make_unique<VertexShader>(gfx, L"PhongVS.cso");

		bindablePtrs.push_back(std::make_unique<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pVS->GetByteCode()));
		bindablePtrs.push_back(std::make_unique<IndexBuffer>(gfx, indices));
		bindablePtrs.push_back(std::make_unique<PixelShader>(gfx, L"PhongPS.cso"));
		bindablePtrs.push_back(std::make_unique<PixelConstantBuffer<Drawable::Material>>(gfx, Drawable::Material{ { 0.6f, 0.6f, 0.8f } }));

		bindablePtrs.push_back(std::move(pVS));
		bindablePtrs.push_back(std::move(pVbuf));

		return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
	}
	std::unique_ptr<Node> ParseNode(const aiNode& node)
	{
		const DirectX::XMMATRIX transform = DirectX::XMMatrixTranspose(DirectX::XMLoadFloat4x4(reinterpret_cast<const DirectX::XMFLOAT4X4*>(&node.mTransformation)));

		std::vector<Mesh*> curMeshPtrs;
		for (UINT i = 0; i < node.mNumMeshes; i++)
			curMeshPtrs.push_back(mMeshPtrs.at(node.mMeshes[i]).get());

		std::unique_ptr<Node> pNode = std::make_unique<Node>(std::move(curMeshPtrs), transform);
		for (UINT i = 0; i < node.mNumChildren; i++)
			pNode->AddChild(ParseNode(*node.mChildren[i]));

		return pNode;
	}

private:
	std::unique_ptr<Node> mRoot;
	std::vector<std::unique_ptr<Mesh>> mMeshPtrs;
};
