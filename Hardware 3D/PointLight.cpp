#include "PointLight.h"
#include "imgui/imgui.h"

PointLight::PointLight(const Graphics& gfx, float radius)
	: mMesh(gfx, radius), mBuffer(gfx)
{
}

void PointLight::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Light"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &mPosition.x, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Y", &mPosition.y, -60.0f, 60.0f, "%.1f");
		ImGui::SliderFloat("Z", &mPosition.z, -60.0f, 60.0f, "%.1f");
		if (ImGui::Button("Reset"))
			Reset();
	}
	ImGui::End();
}

void PointLight::Reset() noexcept
{
	mPosition = {};
}

void PointLight::Draw(const Graphics& gfx) const noexcept(!IS_DEBUG)
{
	mMesh.SetPosition(mPosition);
	mMesh.Draw(gfx);
}

void PointLight::Bind(const Graphics& gfx) const noexcept
{
	mBuffer.Update(gfx, PointLightCBuf{ mPosition });
	mBuffer.Bind(gfx);
}
