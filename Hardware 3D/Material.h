#pragma once

#include <filesystem>
#include <assimp/scene.h>
#include "BindableCommon.h"

class Material
{
public:
	Material(const Graphics& gfx, const aiMaterial& material, const std::filesystem::path& path) noexcept(!IS_DEBUG);

	Dvtx::VertexBuffer ExtractVertices(const aiMesh& mesh) const noexcept;
	std::vector<unsigned short> ExtractIndices(const aiMesh& mesh) const noexcept;

	std::shared_ptr<Bind::VertexBuffer> MakeVertexBindable(const Graphics& gfx, const aiMesh& mesh, float scale = 1.0f) const noexcept(!IS_DEBUG);
	std::shared_ptr<Bind::IndexBuffer> MakeIndexBindable(const Graphics& gfx, const aiMesh& mesh) const noexcept(!IS_DEBUG);

	std::vector<Technique> GetTechniques() const noexcept;

private:
	std::wstring MakeMeshTag(const aiMesh& mesh) const noexcept;

private:
	Dvtx::VertexLayout mVertexLayout;
	std::vector<Technique> mTechniques;
	std::wstring mModelPath;
	std::wstring mName;
};
