#include "TestCube.h"

#include "BindableCommon.h"
#include "Cube.h"
#include "imgui/imgui.h"
#include "TransformCBufDouble.h"

using namespace Bind;
using namespace DirectX;

TestCube::TestCube(const Graphics& gfx, float size)
{
	IndexedTriangleList model = Cube::MakeIndependentTextured();
	model.Transform(XMMatrixScaling(size, size, size));
	model.SetNormalsIndependentFlat();
	const std::wstring geoTag = L"$cube." + std::to_wstring(size);

	std::vector<std::shared_ptr<Bindable>> sharedBinds;
	sharedBinds.push_back(VertexBuffer::Resolve(gfx, geoTag, model.Vertices));
	sharedBinds.push_back(IndexBuffer::Resolve(gfx, geoTag, model.Indices));
	sharedBinds.push_back(Topology::Resolve(gfx, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	sharedBinds.push_back(Rasterizer::Resolve(gfx, false));
	sharedBinds.push_back(Blender::Resolve(gfx, false));
	sharedBinds.push_back(std::make_shared<TransformCBuf>(gfx, *this));

	for (auto& bind : sharedBinds)
	{
		AddBind(bind);
		mOutlineEffect.push_back(std::move(bind));
	}

	AddBind(PixelShader::Resolve(gfx, L"PhongPS.cso"));
	mOutlineEffect.push_back(PixelShader::Resolve(gfx, L"SolidPS.cso"));

	AddBind(std::make_shared<PixelConstantBuffer<Material>>(gfx, mMaterial));
	struct PSOutlineMaterial { XMFLOAT4 MaterialColor = XMFLOAT4{ 1.0f, 0.4f, 0.4f, 1.0f }; };
	mOutlineEffect.push_back(std::make_shared<PixelConstantBuffer<PSOutlineMaterial>>(gfx, PSOutlineMaterial{}));

	std::shared_ptr<VertexShader> pVS = VertexShader::Resolve(gfx, L"PhongVS.cso");
	AddBind(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
	AddBind(std::move(pVS));
	pVS = VertexShader::Resolve(gfx, L"SolidVS.cso");
	mOutlineEffect.push_back(InputLayout::Resolve(gfx, model.Vertices.GetLayout(), pVS->GetByteCode()));
	mOutlineEffect.push_back(std::move(pVS));

	AddBind(Stencil::Resolve(gfx, Stencil::Mode::Write));
	mOutlineEffect.push_back(Stencil::Resolve(gfx, Stencil::Mode::Mask));

	AddBind(Texture::Resolve(gfx, L"Images/brickwall.jpg"));
	AddBind(Sampler::Resolve(gfx));
}

void TestCube::SpawnControlWindow(const Graphics& gfx, const char* name) noexcept
{
	if (ImGui::Begin(name))
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
		bool changed0 = ImGui::ColorPicker3("Spec. Color", reinterpret_cast<float*>(&mMaterial.SpecularColor));
		bool changed1 = ImGui::SliderFloat("Spec. Power", &mMaterial.SpecularPower, 0.0f, 100.0f);

		if (changed0 || changed1)
			QueryBindable<PixelConstantBuffer<Material>>()->Update(gfx, mMaterial);
	}
	ImGui::End();
}

void TestCube::DrawOutline(const Graphics& gfx) noexcept(!IS_DEBUG)
{
	mOutlining = true;
	for (auto& b : mOutlineEffect)
		b->Bind(gfx);
	gfx.DrawIndexed(QueryBindable<Bind::IndexBuffer>()->GetCount());
	mOutlining = false;
}

void XM_CALLCONV TestCube::SetPosition(FXMVECTOR position) noexcept
{
	XMStoreFloat3(&mPosition, position);
}

void TestCube::SetRotation(float roll, float pitch, float yaw) noexcept
{
	mRoll = roll;
	mPitch = pitch;
	mYaw = yaw;
}

XMMATRIX XM_CALLCONV TestCube::GetTransformMatrix() const noexcept
{
	XMMATRIX xf = XMMatrixRotationRollPitchYaw(mPitch, mYaw, mRoll) * XMMatrixTranslationFromVector(XMLoadFloat3(&mPosition));
	if (mOutlining)
		xf = XMMatrixScaling(1.03f, 1.03f, 1.03f) * xf;
	return xf;
}
