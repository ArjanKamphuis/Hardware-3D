#include "Camera.h"

#include "imgui/imgui.h"

using namespace DirectX;

DirectX::XMMATRIX Camera::GetMatrix() const noexcept
{
	return XMMatrixLookAtLH(GetPosition(), XMVectorZero(), XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)) * XMMatrixRotationRollPitchYaw(mPitch, -mYaw, mRoll);
}

DirectX::XMVECTOR Camera::GetPosition() const noexcept
{
	return XMVector3Transform(XMVectorSet(0.0f, 0.0f, -mRadius, 0.0f), XMMatrixRotationRollPitchYaw(mPhi, -mTheta, 0.0f));
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("R", &mRadius, 0.1f, 80.0f, "%.1f");
		ImGui::SliderAngle("Theta", &mTheta, -180.0f, 180.0f);
		ImGui::SliderAngle("Phi", &mPhi, -89.0f, 89.0f);

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &mRoll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &mPitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &mYaw, -180.0f, 180.0f);

		if (ImGui::Button("Reset"))
			Reset();
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	mRadius = 20.0f;
	mTheta = mPhi = mPitch = mYaw = mRoll = 0.0f;
}
