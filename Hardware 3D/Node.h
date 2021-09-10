#pragma once

#include "Graphics.h"

class FrameCommander;
class Mesh;
class Model;

class Node
{
	friend Model;

public:
	Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, DirectX::CXMMATRIX transform) noexcept(!IS_DEBUG);
	void XM_CALLCONV Submit(FrameCommander& frame, DirectX::FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG);
	void XM_CALLCONV SetAppliedTransform(DirectX::FXMMATRIX transform) noexcept;
	const DirectX::XMMATRIX XM_CALLCONV GetAppliedTransform() const noexcept;

	int GetId() const noexcept;
	const std::string& GetName() const;

	bool HasChildren() const noexcept;
	void Accept(class ModelProbe& probe);
	void Accept(class TechniqueProbe& probe);

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
