#pragma once

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include "imgui/imgui.h"

#include <filesystem>
#include <optional>
#include "BindableCommon.h"
#include "Drawable.h"
#include "Vertex.h"

class Mesh : public Drawable
{
public:
	Mesh(const Graphics& gfx, std::vector<std::shared_ptr<Bind::Bindable>> bindPtrs);
	void XM_CALLCONV Draw(const Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override;

private:
	mutable DirectX::XMFLOAT4X4 mTransform = {};
};

class Node
{
	friend class Model;

public:
	struct PSMaterialNormSpec
	{
		BOOL  NormalMapEnabled = TRUE;
		BOOL  SpecularMapEnabled = TRUE;
		BOOL  HasGlossMap = FALSE;
		float SpecularPower = 3.1f;
		DirectX::XMFLOAT3 SpecularColor = { 0.75f, 0.75f, 0.75f };
		float SpecularMapWeight = 0.309f;
	};

	struct PSMaterialNoTexture
	{
		DirectX::XMFLOAT3 MaterialColor = { 0.819608f, 0.576471f, 0.274510f };
		float Padding = 0.0f;
		DirectX::XMFLOAT3 SpecularColor = { 1.0f, 0.972549f, 0.423529f };
		float SpecularPower = 3.25f;
	};

public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, DirectX::CXMMATRIX transform) noexcept(!IS_DEBUG);
	void XM_CALLCONV Draw(const Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void XM_CALLCONV SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMMATRIX& XM_CALLCONV GetAppliedTransform() const noexcept;
	void ShowTree(Node*& pSelectedNode) const noexcept;
	int GetId() const noexcept;

	template<class PSMaterial>
	bool ShowExtraControls(const Graphics& gfx, PSMaterial& material)
	{
		if (mMeshPtrs.empty()) return false;

		if constexpr(std::is_same<PSMaterial, PSMaterialNormSpec>::value)
		{
			if (auto pcb = mMeshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<PSMaterial>>())
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
				return true;
			}
		}
		else if constexpr(std::is_same<PSMaterial, PSMaterialNoTexture>::value)
		{
			if (auto pcb = mMeshPtrs.front()->QueryBindable<Bind::PixelConstantBuffer<PSMaterial>>())
			{
				ImGui::Text("Material");

				ImGui::ColorPicker3("Specular Color", reinterpret_cast<float*>(&material.SpecularColor));
				ImGui::SliderFloat("Specular Power", &material.SpecularPower, 0.0f, 1000.0f, "%.3f", ImGuiSliderFlags_Logarithmic);
				ImGui::ColorPicker3("Diffuse Color", reinterpret_cast<float*>(&material.MaterialColor));

				pcb->Update(gfx, material);
				return true;
			}
		}

		return false;
	}

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);

private:
	int mId;
	std::string mName;
	std::vector<std::unique_ptr<Node>> mChildPtrs;
	std::vector<Mesh*> mMeshPtrs;
	DirectX::XMFLOAT4X4 mTransform;
	DirectX::XMFLOAT4X4 mAppliedTransform;
};

class Model
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception(int line, const wchar_t* file, const std::string& info) noexcept;
		const wchar_t* GetType() const noexcept override;
		std::wstring GetErrorInfo() const noexcept;

	protected:
		void GenerateMessage() const noexcept override;

	private:
		const std::wstring mInfo;
	};

public:
	Model(const Graphics& gfx, const std::string& pathName, float scale = 1.0f);
	~Model() noexcept;
	void Draw(const Graphics& gfx) const noexcept(!IS_DEBUG);
	void ShowWindow(const Graphics& gfx, const char* windowName = nullptr) noexcept;
	void XM_CALLCONV SetRootTransform(DirectX::FXMMATRIX transform) noexcept;

private:
	static std::unique_ptr<Mesh> ParseMesh(const Graphics& gfx, const aiMesh& mesh, const aiMaterial* const* pMaterials, const std::filesystem::path& path, float scale);
	std::unique_ptr<Node> ParseNode(int& nextId, const aiNode& node) noexcept;

private:
	std::unique_ptr<Node> mRoot;
	std::vector<std::unique_ptr<Mesh>> mMeshPtrs;
	std::unique_ptr<class ModelWindow> mWindow;
};
