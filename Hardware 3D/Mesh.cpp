#include "Mesh.h"

#include <stdexcept>
#include <unordered_map>
#include "imgui/imgui.h"
#include "Surface.h"

using namespace Bind;
using namespace DirectX;

Mesh::Mesh(const Graphics& gfx, std::vector<std::shared_ptr<Bindable>> bindPtrs)
{
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));

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

void Node::ShowExtraControls(const Graphics& gfx, PSMaterialNormSpec& material)
{
	if (mMeshPtrs.empty()) return;

	if (auto pcb = mMeshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<PSMaterialNormSpec>>())
	{
		ImGui::Text("Material");

		bool normalMapEnabled = static_cast<bool>(material.NormalMapEnabled);
		ImGui::Checkbox("Normal Map", &normalMapEnabled);
		material.NormalMapEnabled = normalMapEnabled ? TRUE : FALSE;

		bool specularMapEnabled = static_cast<bool>(material.SpecularMapEnabled);
		ImGui::Checkbox("Specular Map", &specularMapEnabled);
		material.SpecularMapEnabled = specularMapEnabled ? TRUE : FALSE;

		bool hasGlossMap = static_cast<bool>(material.HasGlossMap);
		ImGui::Checkbox("Gloss Map", &hasGlossMap);
		material.HasGlossMap = hasGlossMap ? TRUE : FALSE;

		ImGui::SliderFloat("Specular Weight", &material.SpecularMapWeight, 0.0f, 2.0f);
		ImGui::SliderFloat("Specular Power", &material.SpecularPower, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
		ImGui::ColorPicker3("Specular Color", reinterpret_cast<float*>(&material.SpecularColor));

		pcb->Update(gfx, material);
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
	void Show(const Graphics& gfx, const char* windowName, const Node& root) noexcept
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

				mSelectedNode->ShowExtraControls(gfx, mPSMaterial);
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
	Node::PSMaterialNormSpec mPSMaterial;
	std::unordered_map<int, TransformParameters> mTransforms;
};

Model::Model(const Graphics& gfx, std::string filename)
	: mWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(filename.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	if (pScene == nullptr)
		throw Exception(__LINE__, __FILEW__, imp.GetErrorString());

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
		mMeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials));

	int nextId = 0;
	mRoot = ParseNode(nextId, *pScene->mRootNode);
	
	XMStoreFloat4x4(&mRootTransform, XMMatrixIdentity());
}

Model::~Model() noexcept
{
}

void Model::Draw(const Graphics& gfx) const
{
	if (Node* node = mWindow->GetSelectedNode())
		node->SetAppliedTransform(mWindow->GetTransform());
	mRoot->Draw(gfx, XMLoadFloat4x4(&mRootTransform));
}

void Model::ShowWindow(const Graphics& gfx, const char* windowName) noexcept
{
	mWindow->Show(gfx, windowName, *mRoot);
}

void XM_CALLCONV Model::SetRootTransform(DirectX::FXMMATRIX transform) noexcept
{
	XMStoreFloat4x4(&mRootTransform, transform);
}

std::unique_ptr<Mesh> Model::ParseMesh(const Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials)
{
	using Dvtx::VertexLayout;
	using ElementType = VertexLayout::ElementType;

	const wchar_t base[] = L"Models/gobber/";
	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	bool hasSpecularMap = false;
	bool hasAlphaGloss = false;
	bool hasNormalMap = false;
	bool hasDiffuseMap = false;
	float shininess = 2.0f;
	XMFLOAT3 specularColor = { 0.18f, 0.18f, 0.18f };
	XMFLOAT3 diffuseColor = { 0.45f, 0.45f, 0.45f };

	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString texFileName;
		std::string filename;

		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
		{
			filename = texFileName.C_Str();
			bindablePtrs.push_back(Texture::Resolve(gfx, base + std::wstring(filename.begin(), filename.end())));
			hasDiffuseMap = true;
		}
		else
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			filename = texFileName.C_Str();
			auto tex = Texture::Resolve(gfx, base + std::wstring(filename.begin(), filename.end()), 1u);
			hasAlphaGloss = tex->HasAlpha();
			bindablePtrs.push_back(std::move(tex));
			hasSpecularMap = true;
		}
		else
			material.Get(AI_MATKEY_COLOR_SPECULAR, reinterpret_cast<aiColor3D&>(specularColor));

		if (!hasAlphaGloss)
			material.Get(AI_MATKEY_SHININESS, shininess);

		if (material.GetTexture(aiTextureType_NORMALS, 0, &texFileName) == aiReturn_SUCCESS)
		{
			filename = texFileName.C_Str();
			auto tex = Texture::Resolve(gfx, base + std::wstring(filename.begin(), filename.end()), 2u);
			hasAlphaGloss = tex->HasAlpha();
			bindablePtrs.push_back(std::move(tex));
			hasNormalMap = true;
		}

		if (hasDiffuseMap || hasSpecularMap || hasNormalMap)
			bindablePtrs.push_back(Sampler::Resolve(gfx));
	}

	const std::string meshName{ mesh.mName.C_Str() };
	const std::wstring meshTag = std::wstring{ base } + L"%" + std::wstring{ meshName.begin(), meshName.end() };
	const float scale = 6.0f;

	if (hasDiffuseMap && hasNormalMap && hasSpecularMap)
	{
		Dvtx::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(ElementType::Position3D)
			.Append(ElementType::Normal)
			.Append(ElementType::Tangent)
			.Append(ElementType::BiTangent)
			.Append(ElementType::Texture2D)
		));

		for (UINT i = 0; i < mesh.mNumVertices; i++)
			vbuf.EmplaceBack(
				XMFLOAT3{ mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale },
				*reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);

		std::vector<USHORT> indices;
		indices.reserve(static_cast<size_t>(mesh.mNumFaces) * 3u);
		for (UINT i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			for (int j = 0; j < 3; j++)
				indices.push_back(face.mIndices[j]);
		}

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"PhongVSNormalMap.cso");
		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pVS->GetByteCode()));
		bindablePtrs.push_back(std::move(pVS));
		bindablePtrs.push_back(PixelShader::Resolve(gfx, L"PhongPSSpecNormalMap.cso"));

		Node::PSMaterialNormSpec materialConstant;
		materialConstant.SpecularPower = shininess;
		materialConstant.HasGlossMap = hasAlphaGloss ? TRUE : FALSE;

		bindablePtrs.push_back(PixelConstantBuffer<Node::PSMaterialNormSpec>::Resolve(gfx, materialConstant));
	}
	else if (hasDiffuseMap && hasNormalMap)
	{
		Dvtx::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(ElementType::Position3D)
			.Append(ElementType::Normal)
			.Append(ElementType::Tangent)
			.Append(ElementType::BiTangent)
			.Append(ElementType::Texture2D)
		));

		for (UINT i = 0; i < mesh.mNumVertices; i++)
			vbuf.EmplaceBack(
				XMFLOAT3{ mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale },
				*reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<XMFLOAT3*>(&mesh.mTangents[i]),
				*reinterpret_cast<XMFLOAT3*>(&mesh.mBitangents[i]),
				*reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);

		std::vector<USHORT> indices;
		indices.reserve(static_cast<size_t>(mesh.mNumFaces) * 3u);
		for (UINT i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			for (int j = 0; j < 3; j++)
				indices.push_back(face.mIndices[j]);
		}

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"PhongVSNormalMap.cso");
		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pVS->GetByteCode()));
		bindablePtrs.push_back(std::move(pVS));
		bindablePtrs.push_back(PixelShader::Resolve(gfx, L"PhongPSNormalMap.cso"));

		struct Material
		{
			float SpecularIntensity = 0.0f;
			float SpecularPower = 0.0f;
			BOOL  NormalMapEnabled = TRUE;
			float Padding = 0.0f;
		} materialConstant;
		materialConstant.SpecularPower = shininess;
		materialConstant.SpecularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		bindablePtrs.push_back(PixelConstantBuffer<Material>::Resolve(gfx, materialConstant));
	}
	else if (hasDiffuseMap)
	{
		Dvtx::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(ElementType::Position3D)
			.Append(ElementType::Normal)
			.Append(ElementType::Texture2D)
		));

		for (UINT i = 0; i < mesh.mNumVertices; i++)
			vbuf.EmplaceBack(
				XMFLOAT3{ mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale },
				*reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i]),
				*reinterpret_cast<XMFLOAT2*>(&mesh.mTextureCoords[0][i])
			);

		std::vector<USHORT> indices;
		indices.reserve(static_cast<size_t>(mesh.mNumFaces) * 3u);
		for (UINT i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			for (int j = 0; j < 3; j++)
				indices.push_back(face.mIndices[j]);
		}

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"PhongVS.cso");
		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pVS->GetByteCode()));
		bindablePtrs.push_back(std::move(pVS));
		bindablePtrs.push_back(PixelShader::Resolve(gfx, L"PhongPS.cso"));

		struct Material
		{
			float SpecularIntensity = 0.0f;
			float SpecularPower = 0.0f;
			float Padding[2] = {};
		} materialConstant;
		materialConstant.SpecularPower = shininess;
		materialConstant.SpecularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;

		bindablePtrs.push_back(PixelConstantBuffer<Material>::Resolve(gfx, materialConstant));
	}
	else if (!hasDiffuseMap && !hasNormalMap && !hasSpecularMap)
	{
		Dvtx::VertexBuffer vbuf(std::move(
			VertexLayout{}
			.Append(ElementType::Position3D)
			.Append(ElementType::Normal)
		));

		for (UINT i = 0; i < mesh.mNumVertices; i++)
			vbuf.EmplaceBack(
				XMFLOAT3{ mesh.mVertices[i].x * scale, mesh.mVertices[i].y * scale, mesh.mVertices[i].z * scale },
				*reinterpret_cast<XMFLOAT3*>(&mesh.mNormals[i])
			);

		std::vector<USHORT> indices;
		indices.reserve(static_cast<size_t>(mesh.mNumFaces) * 3u);
		for (UINT i = 0; i < mesh.mNumFaces; i++)
		{
			const auto& face = mesh.mFaces[i];
			assert(face.mNumIndices == 3);
			for (int j = 0; j < 3; j++)
				indices.push_back(face.mIndices[j]);
		}

		bindablePtrs.push_back(VertexBuffer::Resolve(gfx, meshTag, vbuf));
		bindablePtrs.push_back(IndexBuffer::Resolve(gfx, meshTag, indices));

		std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"PhongVSNoTex.cso");
		bindablePtrs.push_back(InputLayout::Resolve(gfx, vbuf.GetLayout(), pVS->GetByteCode()));
		bindablePtrs.push_back(std::move(pVS));
		bindablePtrs.push_back(PixelShader::Resolve(gfx, L"PhongPSNoTex.cso"));

		struct Material
		{
			DirectX::XMFLOAT3 MaterialColor = {};
			float SpecularIntensity = 0.0f;
			float SpecularPower = 0.0f;
			float Padding[3] = {};
		} materialConstant;
		materialConstant.SpecularPower = shininess;
		materialConstant.SpecularIntensity = (specularColor.x + specularColor.y + specularColor.z) / 3.0f;
		materialConstant.MaterialColor = diffuseColor;

		bindablePtrs.push_back(PixelConstantBuffer<Material>::Resolve(gfx, materialConstant));
	}
	else
		throw std::runtime_error("Terrible combination of textures in material smh");

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
