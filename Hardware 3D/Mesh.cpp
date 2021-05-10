#include "Mesh.h"

#include "imgui/imgui.h"

using namespace Bind;
using namespace DirectX;

Mesh::Mesh(const Graphics& gfx, std::vector<std::unique_ptr<Bindable>> bindPtrs)
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

void Mesh::Update(float dt) noexcept
{
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

void Mesh::StaticInitialize(const Graphics& gfx)
{
	AddStaticBind(std::make_unique<Topology>(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
}

Node::Node(const std::string& name, std::vector<Mesh*> meshPtrs, CXMMATRIX transform) noexcept(!IS_DEBUG)
	: mName(name), mMeshPtrs(std::move(meshPtrs))
{
	XMStoreFloat4x4(&mTransform, transform);
}

void XM_CALLCONV Node::Draw(const Graphics& gfx, FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	const XMMATRIX built = XMLoadFloat4x4(&mTransform) * accumulatedTransform;

	for (const Mesh* const pm : mMeshPtrs)
		pm->Draw(gfx, built);
	for (const auto& pc : mChildPtrs)
		pc->Draw(gfx, built);
}

void Node::ShowTree() const noexcept
{
	if (ImGui::TreeNode(mName.c_str()))
	{
		for (const auto& pChild : mChildPtrs)
			pChild->ShowTree();
		ImGui::TreePop();
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	mChildPtrs.emplace_back(std::move(pChild));
}

class ModelWindow
{
public:
	void Show(const char* windowName, const Node& root) noexcept
	{
		windowName = windowName ? windowName : "Model";
		if (ImGui::Begin(windowName))
		{
			ImGui::Columns(2);
			root.ShowTree();

			ImGui::NextColumn();

			ImGui::Text("Orientation");
			ImGui::SliderAngle("Roll", &mTransformParams.Roll, -180.0f, 180.0f);
			ImGui::SliderAngle("Pitch", &mTransformParams.Pitch, -180.0f, 180.0f);
			ImGui::SliderAngle("Yaw", &mTransformParams.Yaw, -180.0f, 180.0f);

			ImGui::Text("Position");
			ImGui::SliderFloat("X", &mTransformParams.Position.x, -20.0f, 20.0f);
			ImGui::SliderFloat("Y", &mTransformParams.Position.y, -20.0f, 20.0f);
			ImGui::SliderFloat("Z", &mTransformParams.Position.z, -20.0f, 20.0f);

			if (ImGui::Button("Reset"))
				mTransformParams = {};
		}
		ImGui::End();
	}
	XMMATRIX XM_CALLCONV GetTransform() const noexcept
	{
		return XMMatrixRotationRollPitchYaw(mTransformParams.Roll, mTransformParams.Pitch, mTransformParams.Yaw) * XMMatrixTranslationFromVector(XMLoadFloat3(&mTransformParams.Position));
	}

private:
	struct
	{
		float Roll = 0.0f;
		float Pitch = 0.0f;
		float Yaw = 0.0f;
		DirectX::XMFLOAT3 Position = {};
	} mTransformParams = {};
};

Model::Model(const Graphics& gfx, std::string filename)
	: mWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
		mMeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i]));

	mRoot = ParseNode(*pScene->mRootNode);
}

Model::~Model() noexcept
{
}

void XM_CALLCONV Model::Draw(const Graphics& gfx) const
{
	mRoot->Draw(gfx, mWindow->GetTransform());
}

void Model::ShowWindow(const char* windowName) noexcept
{
	mWindow->Show(windowName, *mRoot);
}

std::unique_ptr<Mesh> Model::ParseMesh(const Graphics& gfx, const aiMesh& mesh)
{
	using Dvtx::VertexLayout;
	using ElementType = VertexLayout::ElementType;

	Dvtx::VertexBuffer vbuf(std::move(
		VertexLayout{}
		.Append(ElementType::Position3D)
		.Append(ElementType::Normal)
	));

	for (UINT i = 0; i < mesh.mNumVertices; i++)
		vbuf.EmplaceBack(*reinterpret_cast<XMFLOAT3*>(&mesh.mVertices[i]), *reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]));

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

std::unique_ptr<Node> Model::ParseNode(const aiNode& node) noexcept
{
	const XMMATRIX transform = XMMatrixTranspose(XMLoadFloat4x4(reinterpret_cast<const XMFLOAT4X4*>(&node.mTransformation)));

	std::vector<Mesh*> curMeshPtrs;
	for (UINT i = 0; i < node.mNumMeshes; i++)
		curMeshPtrs.push_back(mMeshPtrs.at(node.mMeshes[i]).get());

	std::unique_ptr<Node> pNode = std::make_unique<Node>(node.mName.C_Str(), std::move(curMeshPtrs), transform);
	for (UINT i = 0; i < node.mNumChildren; i++)
		pNode->AddChild(ParseNode(*node.mChildren[i]));

	return pNode;
}
