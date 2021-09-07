#include "Node.h"

#include "imgui/imgui.h"
#include "Mesh.h"
#include "ModelProbe.h"

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

int Node::GetId() const noexcept
{
	return mId;
}

const std::string& Node::GetName() const
{
	return mName;
}

bool Node::HasChildren() const noexcept
{
	return !mChildPtrs.empty();
}

void Node::Accept(ModelProbe& probe)
{
	if (probe.PushNode(*this))
	{
		for (auto& cp : mChildPtrs)
			cp->Accept(probe);
		probe.PopNode(*this);
	}
}

void Node::AddChild(std::unique_ptr<Node> pChild) noexcept(!IS_DEBUG)
{
	assert(pChild);
	mChildPtrs.emplace_back(std::move(pChild));
}
