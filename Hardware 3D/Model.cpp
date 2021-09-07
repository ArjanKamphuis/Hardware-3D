#include "Model.h"

#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/scene.h>

#include "Mesh.h"
#include "ModelException.h"
#include "ModelWindow.h"
#include "Node.h"

using namespace DirectX;

Model::Model(const Graphics& gfx, const std::string& pathName, float scale)
	: mWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(pathName.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	if (pScene == nullptr)
		throw ModelException(__LINE__, __FILEW__, imp.GetErrorString());

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
		mMeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, pathName, scale));

	int nextId = 0;
	mRoot = ParseNode(nextId, *pScene->mRootNode);
}

Model::~Model() noexcept
{
}

void Model::Submit(FrameCommander& frame) const noexcept(!IS_DEBUG)
{
	mWindow->ApplyParameters();
	mRoot->Submit(frame, XMMatrixIdentity());
}

void Model::ShowWindow(const Graphics& gfx, const char* windowName) noexcept
{
	mWindow->Show(gfx, windowName, *mRoot);
}

void XM_CALLCONV Model::SetRootTransform(FXMMATRIX transform) noexcept
{
	mRoot->SetAppliedTransform(transform);
}

std::unique_ptr<Mesh> Model::ParseMesh(const Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale)
{
	return {};
}

std::unique_ptr<Node> Model::ParseNode(int& nextId, const aiNode& node) noexcept
{
	const XMMATRIX transform = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node.mTransformation)));

	std::vector<Mesh*> curMeshPtrs;
	for (UINT i = 0; i < node.mNumMeshes; i++)
		curMeshPtrs.push_back(mMeshPtrs.at(node.mMeshes[i]).get());

	std::unique_ptr<Node> pNode = std::make_unique<Node>(nextId++, node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (UINT i = 0; i < node.mNumChildren; i++)
		pNode->AddChild(ParseNode(nextId, *node.mChildren[i]));

	return pNode;
}
