#pragma once

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include "BindableCommon.h"
#include "DrawableBase.h"
#include "Vertex.h"

class Mesh : public DrawableBase<Mesh>
{
public:
	Mesh(const Graphics& gfx, std::vector<std::unique_ptr<Bind::Bindable>> bindPtrs);
	void Update(float dt) noexcept override;
	void XM_CALLCONV Draw(const Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override;

private:
	void StaticInitialize(const Graphics& gfx) override;

private:
	mutable DirectX::XMFLOAT4X4 mTransform = {};
};

class Node
{
	friend class Model;

public:
	Node(std::vector<Mesh*> meshPtrs, DirectX::CXMMATRIX transform) noexcept(!IS_DEBUG);
	void XM_CALLCONV Draw(const Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);

private:
	std::vector<std::unique_ptr<Node>> mChildPtrs;
	std::vector<Mesh*> mMeshPtrs;
	DirectX::XMFLOAT4X4 mTransform;
};

class Model
{
public:
	Model(const Graphics& gfx, std::string filename);
	void XM_CALLCONV Draw(const Graphics& gfx, DirectX::FXMMATRIX transform) const;

private:
	static std::unique_ptr<Mesh> ParseMesh(const Graphics& gfx, const aiMesh& mesh);
	std::unique_ptr<Node> ParseNode(const aiNode& node);

private:
	std::unique_ptr<Node> mRoot;
	std::vector<std::unique_ptr<Mesh>> mMeshPtrs;
};
