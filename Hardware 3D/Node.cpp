#include "Node.h"

#include "imgui/imgui.h"
#include "Mesh.h"

using namespace DirectX;

Node::Node(int id, const std::string& name, std::vector<Mesh*> meshPtrs, CXMMATRIX transform) noexcept(!IS_DEBUG)
	: mId(id), mName(name), mMeshPtrs(std::move(meshPtrs))
{
	XMStoreFloat4x4(&mTransform, transform);
	XMStoreFloat4x4(&mAppliedTransform, XMMatrixIdentity());
}

void XM_CALLCONV Node::Submit(FrameCommander& frame, FXMMATRIX accumulatedTransform) const noexcept(!IS_DEBUG)
{
	const XMMATRIX built = XMLoadFloat4x4(&mAppliedTransform) * XMLoadFloat4x4(&mTransform) * accumulatedTransform;

	for (const Mesh* const pm : mMeshPtrs)
		pm->Submit(frame, built);
	for (const auto& pc : mChildPtrs)
		pc->Submit(frame, built);
}

void XM_CALLCONV Node::SetAppliedTransform(FXMMATRIX transform) noexcept
{
	XMStoreFloat4x4(&mAppliedTransform, transform);
}

const XMMATRIX XM_CALLCONV Node::GetAppliedTransform() const noexcept
{
	return XMLoadFloat4x4(&mAppliedTransform);
}

//void Node::ShowTree(Node*& pSelectedNode) const noexcept
//{
//	const int id = GetId();
//	const int selectedId = pSelectedNode == nullptr ? -1 : pSelectedNode->GetId();
//
//	const auto nodeFlags = ImGuiTreeNodeFlags_OpenOnArrow
//		| (id == selectedId ? ImGuiTreeNodeFlags_Selected : 0)
//		| (mChildPtrs.empty() ? ImGuiTreeNodeFlags_Leaf : 0);
//	const bool expanded = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<intptr_t>(id)), nodeFlags, mName.c_str());
//
//	if (ImGui::IsItemClicked())
//		pSelectedNode = const_cast<Node*>(this);
//
//	if (expanded)
//	{
//		for (const auto& pChild : mChildPtrs)
//			pChild->ShowTree(pSelectedNode);
//		ImGui::TreePop();
//	}
//}

int Node::GetId() const noexcept
{
	return mId;
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	mChildPtrs.emplace_back(std::move(pChild));
}
