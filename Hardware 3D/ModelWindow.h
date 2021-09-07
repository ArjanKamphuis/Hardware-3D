#pragma once

#include <optional>
#include <unordered_map>
#include "DynamicConstantBuffer.h"
#include "Graphics.h"

class Node;

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

	struct NodeData
	{
		TransformParameters TransformParams;
		bool TransformParamsDirty = false;
		std::optional<Dcb::Buffer> MaterialCBuf;
		bool MaterialCBufDirty = false;
	};

public:
	void Show(const Graphics& gfx, const char* windowName, const Node& root) noexcept;
	void ApplyParameters() noexcept(!IS_DEBUG);
private:
	DirectX::XMMATRIX XM_CALLCONV GetTransform() const noexcept;
	const Dcb::Buffer& GetMaterial() const noexcept(!IS_DEBUG);
	void ResetTransformDirty() noexcept(!IS_DEBUG);
	void ResetMaterialDirty() noexcept(!IS_DEBUG);
	bool TransformDirty() const noexcept(!IS_DEBUG);
	bool MaterialDirty() const noexcept(!IS_DEBUG);
	bool IsDirty() const noexcept(!IS_DEBUG);

private:
	Node* mSelectedNode = nullptr;
	std::unordered_map<int, NodeData> mTransforms;
};
