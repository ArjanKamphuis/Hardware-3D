#include "Mesh.h"

#include <unordered_map>
#include "imgui/imgui.h"
#include "Surface.h"

using namespace Bind;
using namespace DirectX;

Mesh::Mesh(const Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs)
{
	AddBind(std::make_shared<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

	for (auto& pb : bindPtrs)
		AddBind(std::move(pb));

	AddBind(std::make_shared<TransformCBuf>(gfx, *this));
}

void XM_CALLCONV Mesh::Draw(const Graphics& gfx, FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	XMStoreFloat4x4(&mTransform, accumulatedTransform);
	Drawable::Draw(gfx);
}

XMMATRIX XM_CALLCONV Mesh::GetTransformMatrix() const noexcept
{
	return XMLoadFloat4x4(&mTransform);
}

Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, CXMMATRIX transform) noexcept(!IS_DEBUG)
	: mId(id), mName(name), mMeshPtrs(std::move(meshPtrs))
{
	XMStoreFloat4x4(&mTransform, transform);
	XMStoreFloat4x4(&mAppliedTransform, XMMatrixIdentity());
}

void XM_CALLCONV Node::Draw(const Graphics& gfx, FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	const XMMATRIX built = XMLoadFloat4x4(&mAppliedTransform) * XMLoadFloat4x4(&mTransform) * accumulatedTransform;

	for (const Mesh* const pm : mMeshPtrs)
		pm->Draw(gfx, built);
	for (const auto& pc : mChildPtrs)
		pc->Draw(gfx, built);
}

void XM_CALLCONV Node::SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept
{
	XMStoreFloat4x4(&mAppliedTransform, transform);
}

void Node::ShowTree(Node*& pSelectedNode) const noexcept
{
	const int id = GetId();
	const int selectedId = pSelectedNode == nullptr ? -1 : pSelectedNode->GetId();

	const auto nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
		| (id == selectedId ? ImGuiTreeNodeFlags_Selected : 0)
		| (mChildPtrs.empty() ? ImGuiTreeNodeFlags_Leaf : 0);
	const bool expanded = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(id)), nodeFlags, mName.c_str());

	if (ImGui::IsItemClicked())
		pSelectedNode = const_cast<Node*>(this);

	if (expanded)
	{
		for (const auto& pChild : mChildPtrs)
			pChild->ShowTree(pSelectedNode);
		ImGui::TreePop();
	}
}

int Node::GetId() const noexcept
{
	return mId;
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	mChildPtrs.emplace_back(std::move(pChild));
}

class ModelWindow
{
private:
	struct TransformParameters
	{
		float Roll = 0.0f;
		float Pitch = 0.0f;
		float Yaw = 0.0f;
		DirectX::XMFLOAT3 Position = {};
	};

public:
	void Show(const char* windowName, const Node& root) noexcept
	{
		windowName = windowName ? windowName : "Model";

		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2);
			root.ShowTree(mSelectedNode);

			ImGui::NextColumn();

			if (mSelectedNode != nullptr)
			{
				TransformParameters& transform = mTransforms[mSelectedNode->GetId()];

				ImGui::Text("Orientation");
				ImGui::SliderAngle("Roll", &transform.Roll, -180.0f, 180.0f);
				ImGui::SliderAngle("Pitch", &transform.Pitch, -180.0f, 180.0f);
				ImGui::SliderAngle("Yaw", &transform.Yaw, -180.0f, 180.0f);

				ImGui::Text("Position");
				ImGui::SliderFloat("X", &transform.Position.x, -20.0f, 20.0f);
				ImGui::SliderFloat("Y", &transform.Position.y, -20.0f, 20.0f);
				ImGui::SliderFloat("Z", &transform.Position.z, -20.0f, 20.0f);

				if (ImGui::Button("Reset"))
					transform = {};
			}
		}
		ImGui::End();
	}
	XMMATRIX XM_CALLCONV GetTransform() const noexcept
	{
		assert(mSelectedNode != nullptr);
		const TransformParameters& transform = mTransforms.at(mSelectedNode->GetId());
		return XMMatrixRotationRollPitchYaw(transform.Roll, transform.Pitch, transform.Yaw) * XMMatrixTranslationFromVector(XMLoadFloat3(&transform.Position));
	}
	Node* GetSelectedNode() const noexcept
	{
		return mSelectedNode;
	}

private:
	Node* mSelectedNode = nullptr;
	std::unordered_map<int, TransformParameters> mTransforms;
};

Model::Model(const Graphics& gfx, std::string filename)
	: mWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals);
	if (pScene == nullptr)
		throw Exception(__LINE__, __FILEW__, imp.GetErrorString());

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
		mMeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));

	int nextId = 0;
	mRoot = ParseNode(nextId, *pScene->mRootNode);
}

Model::~Model() noexcept
{
}

void XM_CALLCONV Model::Draw(const Graphics& gfx) const
{
	if (Node* node = mWindow->GetSelectedNode())
		node->SetAppliedTransform(mWindow->GetTransform());
	mRoot->Draw(gfx, XMMatrixIdentity());
}

void Model::ShowWindow(const char* windowName) noexcept
{
	mWindow->Show(windowName, *mRoot);
}

std::unique_ptr<Mesh> Model::ParseMesh(const Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using Dvtx::VertexLayout;
	using ElementType = VertexLayout::ElementType;

	Dvtx::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(ElementType::Position3D)
		.Append(ElementType::Normal)
		.Append(ElementType::Texture2D)
	));

	for (UINT i = 0; i < mesh.mNumVertices; i++)
		vbuf.EmplaceBack(
			*reinterpret_cast<XMFLOAT3*>(&mesh.mVertices[i]),
			*reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
			*reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i])
		);

	std::vector<USHORT> indices;
	indices.reserve(static_cast<size_t>(mesh.mNumFaces * 3u));
	for (UINT i = 0; i < mesh.mNumFaces; i++)
	{
		const auto& face = mesh.mFaces[i];
		assert(face.mNumIndices == 3);
		for (int j = 0; j < 3; j++)
			indices.push_back(face.mIndices[j]);
	}

	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	bool hasSpecularMap = false;
	Drawable::Material materialConstant;
	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];
		const wchar_t base[] = L"Models/nano_textured/";
		aiString texFileName;

		material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName);
		std::string filename{ texFileName.C_Str() };
		bindablePtrs.push_back(std::make_shared<Texture>(gfx, Surface::FromFile(base + std::wstring(filename.begin(), filename.end()))));

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			filename = texFileName.C_Str();
			bindablePtrs.push_back(std::make_shared<Texture>(gfx, Surface::FromFile(base + std::wstring(filename.begin(), filename.end())), 1u));
			hasSpecularMap = true;
		}
		else
			material.Get(AI_MATKEY_SHININESS, materialConstant.SpecularPower);

		bindablePtrs.push_back(std::make_shared<Sampler>(gfx));
	}

	std::shared_ptr<VertexBuffer> pVbuf = std::make_shared<VertexBuffer>(gfx, vbuf);
	std::shared_ptr<VertexShader> pVS = std::make_shared<VertexShader>(gfx, L"PhongVS.cso");

	bindablePtrs.push_back(std::make_shared<InputLayout>(gfx, vbuf.GetLayout().GetD3DLayout(), pVS->GetByteCode()));
	bindablePtrs.push_back(std::make_shared<IndexBuffer>(gfx, indices));

	if (hasSpecularMap)
		bindablePtrs.push_back(std::make_shared<PixelShader>(gfx, L"PhongPSSpecMap.cso"));
	else
	{
		bindablePtrs.push_back(std::make_shared<PixelShader>(gfx, L"PhongPS.cso"));
		bindablePtrs.push_back(std::make_shared<PixelConstantBuffer<Drawable::Material>>(gfx, materialConstant));
	}

	bindablePtrs.push_back(std::move(pVS));
	bindablePtrs.push_back(std::move(pVbuf));

	return std::make_unique<Mesh>(gfx, std::move(bindablePtrs));
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
