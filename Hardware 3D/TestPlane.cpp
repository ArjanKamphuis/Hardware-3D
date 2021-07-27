#include "TestPlane.h"

#include "BindableCommon.h"
#include "imgui/imgui.h"
#include "Plane.h"
#include "TransformCBufDouble.h"

using namespace Bind;
using namespace DirectX;

TestPlane::TestPlane(const Graphics& gfx, float size)
{
	IndexedTriangleList model = Plane::Make();
	model.Transform(XMMatrixScaling(size, size, 1.0f));
	const std::wstring geoTag = L"$plane." + std::to_wstring(size);
	
	AddBind(VertexBuffer::Resolve(gfx, geoTag, model.Vertices));
	AddBind(IndexBuffer::Resolve(gfx, geoTag, model.Indices));

	AddBind(PixelShader::Resolve(gfx, L"PhongPSNormalMapObject.cso"));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(PixelConstantBuffer<Material>::Resolve(gfx, mMaterial));

	std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"PhongVS.cso");
	AddBind(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
	AddBind(std::move(pVS));

	AddBind(Texture::Resolve(gfx, L"Images/brickwall.jpg"));
	AddBind(Texture::Resolve(gfx, L"Images/brickwall_normal_obj.png", 2u));
	AddBind(Sampler::Resolve(gfx));

	AddBind(std::make_shared<TransformCBufDouble>(gfx, *this, 0u, 2u));
}

void TestPlane::SpawnControlWindow(const Graphics& gfx) noexcept
{
	if (ImGui::Begin("Plane"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &mPosition.x, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Y", &mPosition.y, -80.0f, 80.0f, "%.1f");
		ImGui::SliderFloat("Z", &mPosition.z, -80.0f, 80.0f, "%.1f");
		
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &mRoll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &mPitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &mYaw, -180.0f, 180.0f);

		ImGui::Text("Shading");
		bool changed0 = ImGui::SliderFloat("Spec. Int.", &mMaterial.SpecularIntensity, 0.0f, 1.0f);
		bool changed1 = ImGui::SliderFloat("Spec. Power", &mMaterial.SpecularPower, 0.0f, 100.0f);
		bool checkState = mMaterial.NormalMapEnabled == TRUE;
		bool changed2 = ImGui::Checkbox("Enable Normal Map", &checkState);
		mMaterial.NormalMapEnabled = checkState ? TRUE : FALSE;

		if (changed0 || changed1 || changed2)
			QueryBindable<PixelConstantBuffer<Material>>()->Update(gfx, mMaterial);
	}
	ImGui::End();
}

void XM_CALLCONV TestPlane::SetPosition(FXMVECTOR position) noexcept
{
	XMStoreFloat3(&mPosition, position);
}

void TestPlane::SetRotation(float roll, float pitch, float yaw) noexcept
{
	mRoll = roll;
	mPitch = pitch;
	mYaw = yaw;
}

XMMATRIX XM_CALLCONV TestPlane::GetTransformMatrix() const noexcept
{
	return XMMatrixRotationRollPitchYaw(mPitch, mYaw, mRoll) * XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
}
