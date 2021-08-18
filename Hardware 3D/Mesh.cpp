#include "Mesh.h"

#include <stdexcept>
#include <unordered_map>
#include "ChiliXM.h"
#include "ChiliUtil.h"
#include "ConstantBuffersEx.h"
#include "LayoutCodex.h"
#include "Surface.h"

using namespace Bind;
using namespace DirectX;
using namespace ChiliXM;
using namespace std::string_literals;
using ChiliUtil::ToWide;

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

void XM_CALLCONV Node::SetAppliedTransform(FXMMATRIX transform) noexcept
{
	XMStoreFloat4x4(&mAppliedTransform, transform);
}

const XMMATRIX XM_CALLCONV Node::GetAppliedTransform() const noexcept
{
	return XMLoadFloat4x4(&mAppliedTransform);
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

const Dcb::Buffer* Node::GetMaterialConstants() const noexcept(!IS_DEBUG)
{
	return mMeshPtrs.size() == 0 ? nullptr : &(mMeshPtrs.front()->QueryBindable<CachingPixelConstantBufferEx>()->GetBuffer());
}

void Node::SetMaterialConstants(const Dcb::Buffer& buffer) noexcept(!IS_DEBUG)
{
	auto pcb = mMeshPtrs.front()->QueryBindable<CachingPixelConstantBufferEx>();
	assert(pcb != nullptr);
	pcb->SetBuffer(buffer);
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
		XMFLOAT3 Position = {};
	};

	struct NodeData
	{
		TransformParameters TransformParams;
		std::optional<Dcb::Buffer> MaterialCBuf;
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
				const int id = mSelectedNode->GetId();
				auto it = mTransforms.find(id);
				if (it == mTransforms.end())
				{
					const XMMATRIX& applied = mSelectedNode->GetAppliedTransform();
					TransformParameters tp;
					XMStoreFloat3(&tp.Position, ExtractTranslation(applied));
					XMFLOAT3 angles;
					XMStoreFloat3(&angles, ExtractEulerAngles(applied));
					tp.Roll = angles.z;
					tp.Pitch = angles.x;
					tp.Yaw = angles.y;
					const Dcb::Buffer* pMatConst = mSelectedNode->GetMaterialConstants();
					auto buf = pMatConst != nullptr ? std::optional<Dcb::Buffer>{ *pMatConst } : std::optional<Dcb::Buffer>{};
					std::tie(it, std::ignore) = mTransforms.insert({ id, { tp, std::move(buf) } });
				}

				TransformParameters& transform = it->second.TransformParams;

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

				if (it->second.MaterialCBuf)
				{
					Dcb::Buffer& mat = *it->second.MaterialCBuf;
					ImGui::Text("Material");

					if (Dcb::ElementRef v = mat[L"NormalMapEnabled"s]; v.Exists()) ImGui::Checkbox("Norm Map", &v);
					if (Dcb::ElementRef v = mat[L"SpecularMapEnabled"s]; v.Exists()) ImGui::Checkbox("Spec Map", &v);
					if (Dcb::ElementRef v = mat[L"HasGlossMap"s]; v.Exists()) ImGui::Checkbox("Gloss Map", &v);
					if (Dcb::ElementRef v = mat[L"MaterialColor"s]; v.Exists()) ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&static_cast<XMFLOAT3&>(v)));
					if (Dcb::ElementRef v = mat[L"SpecularPower"s]; v.Exists()) ImGui::SliderFloat("Spec Power", &v, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic);
					if (Dcb::ElementRef v = mat[L"SpecularColor"s]; v.Exists()) ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&static_cast<XMFLOAT3&>(v)));
					if (Dcb::ElementRef v = mat[L"SpecularMapWeight"s]; v.Exists()) ImGui::SliderFloat("Spec Weight", &v, 0.0f, 4.0f);
					if (Dcb::ElementRef v = mat[L"SpecularIntensity"s]; v.Exists()) ImGui::SliderFloat("Spec Intens", &v, 0.0f, 1.0f);
					
				}
			}
		}
		ImGui::End();
	}
	XMMATRIX XM_CALLCONV GetTransform() const noexcept
	{
		assert(mSelectedNode != nullptr);
		const TransformParameters& transform = mTransforms.at(mSelectedNode->GetId()).TransformParams;
		return XMMatrixRotationRollPitchYaw(transform.Pitch, transform.Yaw, transform.Roll) * XMMatrixTranslationFromVector(XMLoadFloat3(&transform.Position));
	}
	const Dcb::Buffer* GetMaterial() const noexcept
	{
		assert(mSelectedNode != nullptr);
		const auto& mat = mTransforms.at(mSelectedNode->GetId()).MaterialCBuf;
		return mat ? &*mat : nullptr;
	}
	Node* GetSelectedNode() const noexcept
	{
		return mSelectedNode;
	}

private:
	Node* mSelectedNode = nullptr;
	std::unordered_map<int, NodeData> mTransforms;
};

Model::Model(const Graphics& gfx, const std::string& pathName, float scale)
	: mWindow(std::make_unique<ModelWindow>())
{
	Assimp::Importer imp;
	const auto pScene = imp.ReadFile(pathName.c_str(), aiProcess_Triangulate | aiProcess_JoinIdenticalVertices | aiProcess_ConvertToLeftHanded | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
	if (pScene == nullptr)
		throw Exception(__LINE__, __FILEW__, imp.GetErrorString());

	for (size_t i = 0; i < pScene->mNumMeshes; i++)
		mMeshPtrs.push_back(ParseMesh(gfx, *pScene->mMeshes[i], pScene->mMaterials, pathName, scale));

	int nextId = 0;
	mRoot = ParseNode(nextId, *pScene->mRootNode);
}

Model::~Model() noexcept
{
}

void Model::Draw(const Graphics& gfx) const noexcept(!IS_DEBUG)
{
	if (Node* node = mWindow->GetSelectedNode())
	{
		node->SetAppliedTransform(mWindow->GetTransform());
		if (const Dcb::Buffer* mat = mWindow->GetMaterial())
			node->SetMaterialConstants(*mat);
	}
	mRoot->Draw(gfx, XMMatrixIdentity());
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
	using Dvtx::VertexLayout;
	using ElementType = VertexLayout::ElementType;

	const std::wstring rootPath = path.parent_path().generic_wstring() + L"/";
	std::vector<std::shared_ptr<Bindable>> bindablePtrs;

	bool hasSpecularMap = false;
	bool hasAlphaGloss = false;
	bool hasAlphaDiffuse = false;
	bool hasNormalMap = false;
	bool hasDiffuseMap = false;
	float shininess = 2.0f;
	XMFLOAT3 specularColor = { 0.18f, 0.18f, 0.18f };
	XMFLOAT3 diffuseColor = { 0.45f, 0.45f, 0.45f };

	if (mesh.mMaterialIndex >= 0)
	{
		auto& material = *pMaterials[mesh.mMaterialIndex];
		aiString texFileName;

		if (material.GetTexture(aiTextureType_DIFFUSE, 0, &texFileName) == aiReturn_SUCCESS)
		{
			auto tex = Texture::Resolve(gfx, rootPath + ToWide(texFileName.C_Str()));
			hasAlphaDiffuse = tex->HasAlpha();
			bindablePtrs.push_back(std::move(tex));
			hasDiffuseMap = true;
		}
		else
			material.Get(AI_MATKEY_COLOR_DIFFUSE, reinterpret_cast<aiColor3D&>(diffuseColor));

		if (material.GetTexture(aiTextureType_SPECULAR, 0, &texFileName) == aiReturn_SUCCESS)
		{
			auto tex = Texture::Resolve(gfx, rootPath + ToWide(texFileName.C_Str()), 1u);
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
			auto tex = Texture::Resolve(gfx, rootPath + ToWide(texFileName.C_Str()), 2u);
			hasAlphaGloss = tex->HasAlpha();
			bindablePtrs.push_back(std::move(tex));
			hasNormalMap = true;
		}

		if (hasDiffuseMap || hasSpecularMap || hasNormalMap)
			bindablePtrs.push_back(Sampler::Resolve(gfx));
	}

	const std::wstring meshTag = rootPath + L"%" + ToWide(mesh.mName.C_Str());

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
		bindablePtrs.push_back(PixelShader::Resolve(gfx, hasAlphaDiffuse ? L"PhongPSSpecNormMask.cso" : L"PhongPSSpecNormalMap.cso"));

		Dcb::RawLayout layout;
		layout.Add<Dcb::Bool>(L"NormalMapEnabled"s);
		layout.Add<Dcb::Bool>(L"SpecularMapEnabled"s);
		layout.Add<Dcb::Bool>(L"HasGlossMap"s);
		layout.Add<Dcb::Float>(L"SpecularPower"s);
		layout.Add<Dcb::Float3>(L"SpecularColor"s);
		layout.Add<Dcb::Float>(L"SpecularMapWeight"s);
		Dcb::Buffer cbuf = Dcb::Buffer::Make(std::move(layout));
		cbuf[L"NormalMapEnabled"s] = true;
		cbuf[L"SpecularMapEnabled"s] = true;
		cbuf[L"HasGlossMap"s] = false;
		cbuf[L"SpecularPower"s] = shininess;
		cbuf[L"SpecularColor"s] = specularColor;
		cbuf[L"SpecularMapWeight"s] = 0.309f;
		bindablePtrs.push_back(std::make_shared<CachingPixelConstantBufferEx>(gfx, cbuf));
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

		Dcb::RawLayout layout;
		layout.Add<Dcb::Float3>(L"SpecularColor"s);
		layout.Add<Dcb::Float>(L"SpecularPower"s);
		layout.Add<Dcb::Bool>(L"NormalMapEnabled"s);
		Dcb::Buffer cbuf = Dcb::Buffer::Make(std::move(layout));
		cbuf[L"SpecularColor"s] = specularColor;
		cbuf[L"SpecularPower"s] = shininess;
		cbuf[L"NormalMapEnabled"s] = true;
		bindablePtrs.push_back(std::make_shared<CachingPixelConstantBufferEx>(gfx, cbuf));
	}
	else if (hasDiffuseMap && !hasNormalMap && hasSpecularMap)
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
		bindablePtrs.push_back(PixelShader::Resolve(gfx, L"PhongPSSpec.cso"));

		Dcb::RawLayout layout;
		layout.Add<Dcb::Float>(L"HasGloss"s);
		layout.Add<Dcb::Float>(L"SpecularPower"s);
		layout.Add<Dcb::Float>(L"SpecularMapWeight"s);
		Dcb::Buffer cbuf = Dcb::Buffer::Make(std::move(layout));
		cbuf[L"hasGloss"s] = hasAlphaGloss;
		cbuf[L"SpecularPower"s] = shininess;
		cbuf[L"SpecularMapWeight"] = 1.0f;
		bindablePtrs.push_back(std::make_shared<CachingPixelConstantBufferEx>(gfx, cbuf));
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

		Dcb::RawLayout layout;
		layout.Add<Dcb::Float3>(L"SpecularColor"s);
		layout.Add<Dcb::Float>(L"SpecularPower"s);
		Dcb::Buffer cbuf = Dcb::Buffer::Make(std::move(layout));
		cbuf[L"SpecularColor"s] = specularColor;
		cbuf[L"SpecularPower"] = shininess;
		bindablePtrs.push_back(std::make_shared<CachingPixelConstantBufferEx>(gfx, cbuf));
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

		Dcb::RawLayout layout;
		layout.Add<Dcb::Float3>(L"MaterialColor"s);
		layout.Add<Dcb::Float3>(L"SpecularColor"s);
		layout.Add<Dcb::Float>(L"SpecularPower"s);
		Dcb::Buffer cbuf = Dcb::Buffer::Make(std::move(layout));
		cbuf[L"MaterialColor"s] = diffuseColor;
		cbuf[L"SpecularColor"s] = specularColor;
		cbuf[L"SpecularPower"s] = shininess;
		bindablePtrs.push_back(std::make_shared<CachingPixelConstantBufferEx>(gfx, cbuf));
	}
	else
		throw std::runtime_error("Terrible combination of textures in material smh");

	// hasAlphaDiffuse as no-culling works only in sponza
	bindablePtrs.push_back(Rasterizer::Resolve(gfx, hasAlphaDiffuse));
	bindablePtrs.push_back(Blender::Resolve(gfx, false));

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
