#include "PointLight.h"
#include "imgui/imgui.h"

using namespace DirectX;

PointLight::PointLight(const Graphics& gfx, float radius)
	: mMesh(gfx, radius), mBuffer(gfx, 1u)
{
	Reset();
}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &mBufferData.LightPosition.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &mBufferData.LightPosition.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &mBufferData.LightPosition.z, -60.0f, 60.0f, "%.1f");

		ImGui::Text("Intensity/Color");
		ImGui::SliderFloat("Intensity", &mBufferData.DiffuseIntensity, 0.01f, 2.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::ColorEdit3("Diffuse Color", &mBufferData.DiffuseColor.x);
		ImGui::ColorEdit3("Ambient Color", &mBufferData.AmbientColor.x);

		ImGui::Text("Falloff");
		ImGui::SliderFloat("Constant", &mBufferData.AttConst, 0.05f, 10.0f, "%.2f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Linear", &mBufferData.AttLinear, 0.0001f, 4.0f, "%.4f", ImGuiSliderFlags_Logarithmic);
		ImGui::SliderFloat("Quadratic", &mBufferData.AttQuad, 0.000001f, 10.0f, "%.6f", ImGuiSliderFlags_Logarithmic);

		if (ImGui::Button("Reset"))
			Reset();
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	mBufferData =
	{
		{ 0.0f, 0.0f, 0.0f },
		{ 0.0f, 4.0f, -4.5f },
		{ 0.05f, 0.05f, 0.05f },
		{ 1.0f, 1.0f, 1.0f },
		1.0f, 1.0f, 0.045f, 0.0075f
	};
}

void PointLight::Draw(const Graphics& gfx) const noexcept(!IS_DEBUG)
{
	mMesh.SetPosition(XMLoadFloat3(&mBufferData.LightPosition));
	mMesh.SetColor(gfx, XMLoadFloat3(&mBufferData.DiffuseColor));
	mMesh.Draw(gfx);
}

void PointLight::Bind(const Graphics& gfx) const noexcept
{
	mBuffer.Update(gfx, mBufferData);
	mBuffer.Bind(gfx);
}

void XM_CALLCONV PointLight::SetCameraPosition(FXMVECTOR cam) noexcept
{
	XMStoreFloat3(&mBufferData.CameraPosition, cam);
}
