#include "ModelWindow.h"

#include "Node.h"

using namespace DirectX;

void ModelWindow::Show(const Graphics& gfx, const char* windowName, const Node& root) noexcept
{
	/*windowName = windowName ? windowName : "Model";

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
	std::tie(it, std::ignore) = mTransforms.insert({ id, { tp, false, std::move(buf), false } });
	}

	{
	TransformParameters& transform = it->second.TransformParams;
	bool& dirty = it->second.TransformParamsDirty;
	const auto dcheck = [&dirty](bool changed) { dirty = dirty || changed; };

	ImGui::Text("Orientation");
	dcheck(ImGui::SliderAngle("Roll", &transform.Roll, -180.0f, 180.0f));
	dcheck(ImGui::SliderAngle("Pitch", &transform.Pitch, -180.0f, 180.0f));
	dcheck(ImGui::SliderAngle("Yaw", &transform.Yaw, -180.0f, 180.0f));

	ImGui::Text("Position");
	dcheck(ImGui::SliderFloat("X", &transform.Position.x, -20.0f, 20.0f));
	dcheck(ImGui::SliderFloat("Y", &transform.Position.y, -20.0f, 20.0f));
	dcheck(ImGui::SliderFloat("Z", &transform.Position.z, -20.0f, 20.0f));
	}

	if (it->second.MaterialCBuf)
	{
	Dcb::Buffer& mat = *it->second.MaterialCBuf;
	bool& dirty = it->second.MaterialCBufDirty;
	const auto dcheck = [&dirty](bool changed) { dirty = dirty || changed; };

	ImGui::Text("Material");

	if (Dcb::ElementRef v = mat[L"NormalMapEnabled"s]; v.Exists()) dcheck(ImGui::Checkbox("Norm Map", &v));
	if (Dcb::ElementRef v = mat[L"SpecularMapEnabled"s]; v.Exists()) dcheck(ImGui::Checkbox("Spec Map", &v));
	if (Dcb::ElementRef v = mat[L"HasGlossMap"s]; v.Exists()) dcheck(ImGui::Checkbox("Gloss Map", &v));
	if (Dcb::ElementRef v = mat[L"SpecularPower"s]; v.Exists()) dcheck(ImGui::SliderFloat("Spec Power", &v, 0.0f, 100.0f, "%.1f", ImGuiSliderFlags_Logarithmic));
	if (Dcb::ElementRef v = mat[L"SpecularMapWeight"s]; v.Exists()) dcheck(ImGui::SliderFloat("Spec Weight", &v, 0.0f, 4.0f));
	if (Dcb::ElementRef v = mat[L"SpecularIntensity"s]; v.Exists()) dcheck(ImGui::SliderFloat("Spec Intens", &v, 0.0f, 1.0f));
	if (Dcb::ElementRef v = mat[L"MaterialColor"s]; v.Exists()) dcheck(ImGui::ColorPicker3("Diff Color", reinterpret_cast<float*>(&static_cast<XMFLOAT3&>(v))));
	if (Dcb::ElementRef v = mat[L"SpecularColor"s]; v.Exists()) dcheck(ImGui::ColorPicker3("Spec Color", reinterpret_cast<float*>(&static_cast<XMFLOAT3&>(v))));
	}
	}
	}
	ImGui::End();*/
}

void ModelWindow::ApplyParameters() noexcept(!IS_DEBUG)
{
	/*if (TransformDirty())
	{
	mSelectedNode->SetAppliedTransform(GetTransform());
	ResetTransformDirty();
	}
	if (MaterialDirty())
	{
	mSelectedNode->SetMaterialConstants(GetMaterial());
	ResetMaterialDirty();
	}*/
}

DirectX::XMMATRIX XM_CALLCONV ModelWindow::GetTransform() const noexcept
{
	//assert(mSelectedNode != nullptr);
	//const TransformParameters& transform = mTransforms.at(mSelectedNode->GetId()).TransformParams;
	//return XMMatrixRotationRollPitchYaw(transform.Pitch, transform.Yaw, transform.Roll) * XMMatrixTranslationFromVector(XMLoadFloat3(&transform.Position));
	return XMMatrixIdentity();
}

//const Dcb::Buffer& ModelWindow::GetMaterial() const noexcept(!IS_DEBUG)
//{
//	assert(mSelectedNode != nullptr);
//	const auto& mat = mTransforms.at(mSelectedNode->GetId()).MaterialCBuf;
//	assert(mat);
//	return *mat;
//}

void ModelWindow::ResetTransformDirty() noexcept(!IS_DEBUG)
{
	//mTransforms.at(mSelectedNode->GetId()).TransformParamsDirty = false;
}

void ModelWindow::ResetMaterialDirty() noexcept(!IS_DEBUG)
{
	//mTransforms.at(mSelectedNode->GetId()).MaterialCBufDirty = false;
}

bool ModelWindow::TransformDirty() const noexcept(!IS_DEBUG)
{
	return false;// mSelectedNode&& mTransforms.at(mSelectedNode->GetId()).TransformParamsDirty;
}

bool ModelWindow::MaterialDirty() const noexcept(!IS_DEBUG)
{
	return false;// mSelectedNode&& mTransforms.at(mSelectedNode->GetId()).MaterialCBufDirty;
}

bool ModelWindow::IsDirty() const noexcept(!IS_DEBUG)
{
	return TransformDirty() || MaterialDirty();
}
