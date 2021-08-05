#include "TestPlane.h"

#include "BindableCommon.h"
#include "imgui/imgui.h"
#include "Plane.h"
#include "TransformCBufDouble.h"

using namespace Bind;
using namespace DirectX;

TestPlane::TestPlane(const Graphics& gfx, float size, DirectX::FXMVECTOR color)
{
	XMStoreFloat4(&mMaterial.Color, color);

	IndexedTriangleList model = Plane::Make();
	model.Transform(XMMatrixScaling(size, size, 1.0f));
	const std::wstring geoTag = L"$plane." + std::to_wstring(size);
	
	AddBind(VertexBuffer::Resolve(gfx, geoTag, model.Vertices));
	AddBind(IndexBuffer::Resolve(gfx, geoTag, model.Indices));

	AddBind(PixelShader::Resolve(gfx, L"SolidPS.cso"));
	AddBind(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(PixelConstantBuffer<Material>::Resolve(gfx, mMaterial));

	std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"SolidVS.cso");
	AddBind(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
	AddBind(std::move(pVS));

	AddBind(std::make_shared<TransformCBufDouble>(gfx, *this, 0u, 2u));

	AddBind(Blender::Resolve(gfx, true, 0.5f));
	AddBind(Rasterizer::Resolve(gfx, true));
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
		Blender* pBlender = QueryBindable<Blender>();
		float factor = pBlender->GetBlendFactor();
		ImGui::SliderFloat("Translucency", &factor, 0.0f, 1.0f);
		pBlender->SetBlendFactor(factor);
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
