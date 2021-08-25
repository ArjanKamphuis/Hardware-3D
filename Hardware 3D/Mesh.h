#pragma once

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include "imgui/imgui.h"

#include <filesystem>
#include <optional>
#include "BindableCommon.h"
#include "Drawable.h"
#include "DynamicConstantBuffer.h"
#include "Vertex.h"

class Mesh : public Drawable
{
public:
	using Drawable::Drawable;
	void XM_CALLCONV Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	DirectX::XMMATRIX XM_CALLCONV GetTransformMatrix() const noexcept override;

private:
	mutable DirectX::XMFLOAT4X4 mTransform = {};
};

class Node
{
	friend class Model;

public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, DirectX::CXMMATRIX transform) noexcept(!IS_DEBUG);
	void XM_CALLCONV Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void XM_CALLCONV SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMMATRIX XM_CALLCONV GetAppliedTransform() const noexcept;
	void ShowTree(Node*& pSelectedNode) const noexcept;
	int GetId() const noexcept;

	//const Dcb::Buffer* GetMaterialConstants() const noexcept(!IS_DEBUG);
	//void SetMaterialConstants(const Dcb::Buffer& buffer) noexcept(!IS_DEBUG);

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
	void Submit(FrameCommander& frame) const noexcept(!IS_DEBUG);
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
