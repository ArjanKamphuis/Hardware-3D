#pragma once

#include <filesystem>
#include "ChiliException.h"
#include "Graphics.h"

class FrameCommander;
class Mesh;
class ModelProbe;
class Node;
struct aiMaterial;
struct aiMesh;
struct aiNode;

class Model
{
public:
	Model(const Graphics& gfx, const std::string& pathName, float scale = 1.0f);
	~Model() noexcept;
	void Submit(FrameCommander& frame) const noexcept(!IS_DEBUG);
	void Accept(ModelProbe& probe);
	void XM_CALLCONV SetRootTransform(DirectX::FXMMATRIX transform) noexcept;

private:
	static std::unique_ptr<Mesh> ParseMesh(const Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	std::unique_ptr<Node> XM_CALLCONV ParseNode(int& nextId, const aiNode& node, DirectX::FXMMATRIX additionalTransform) noexcept;

private:
	std::unique_ptr<Node> mRoot;
	std::vector<std::unique_ptr<Mesh>> mMeshPtrs;
};
