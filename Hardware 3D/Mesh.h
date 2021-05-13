#pragma once

#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>

#include <optional>
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
	friend class ModelWindow;

public:
	Node(const std::string& name, std::vector<Mesh*> meshPtrs, DirectX::CXMMATRIX transform) noexcept(!IS_DEBUG);
	void XM_CALLCONV Draw(const Graphics& gfx, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void XM_CALLCONV SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;

private:
	void AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG);
	void ShowTree(int& nodeIndex, std::optional<int>& selectedIndex, Node*& pSelectedNode) const noexcept;

private:
	std::string mName;
	std::vector<std::unique_ptr<Node>> mChildPtrs;
	std::vector<Mesh*> mMeshPtrs;
	DirectX::XMFLOAT4X4 mBaseTransform;
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
	Model(const Graphics& gfx, std::string filename);
	~Model() noexcept;
	void XM_CALLCONV Draw(const Graphics& gfx) const noexcept(!IS_DEBUG);
	void ShowWindow(const char* windowName = nullptr) noexcept;

private:
	static std::unique_ptr<Mesh> ParseMesh(const Graphics& gfx, const aiMesh& mesh);
	std::unique_ptr<Node> ParseNode(const aiNode& node) noexcept;

private:
	std::unique_ptr<Node> mRoot;
	std::vector<std::unique_ptr<Mesh>> mMeshPtrs;
	std::unique_ptr<class ModelWindow> mWindow;
};
