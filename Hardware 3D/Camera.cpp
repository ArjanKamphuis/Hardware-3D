#include "Camera.h"

#include <algorithm>
#include "imgui/imgui.h"

using namespace DirectX;

Camera::Camera() noexcept
{
	Reset();
}

DirectX::XMMATRIX XM_CALLCONV Camera::GetMatrix() const noexcept
{
	const XMMATRIX transform = XMMatrixRotationRollPitchYaw(mPitch, mYaw, 0.0f);
	return XMMatrixLookToLH(GetPosition(), XMVector3Transform(mForward, transform), XMVector3Transform(mUp, transform));
}

DirectX::XMVECTOR XM_CALLCONV Camera::GetPosition() const noexcept
{
	return XMLoadFloat3(&mPosition);
}

void Camera::SpawnControlWindow() noexcept
{
	if (ImGui::Begin("Camera"))
	{
		ImGui::Text("Position");
		ImGui::SliderFloat("X", &mPosition.x, -mMaxRadius, mMaxRadius, "%.1f");
		ImGui::SliderFloat("Y", &mPosition.y, -mMaxRadius, mMaxRadius, "%.1f");
		ImGui::SliderFloat("Z", &mPosition.z, -mMaxRadius, mMaxRadius, "%.1f");

		ImGui::Text("Orientation");
		ImGui::SliderAngle("Pitch", &mPitch, -90.0f, 90.0f);
		ImGui::SliderAngle("Yaw", &mYaw, -180.0f, 180.0f);

		if (ImGui::Button("Reset"))
			Reset();
	}
	ImGui::End();
}

void Camera::Reset() noexcept
{
	mPosition = { 0.0f, 0.0f, -3.0f };
	mPitch = 0.0f;
	mYaw = 0.0f;
}

void Camera::Rotate(float dx, float dy) noexcept
{
	mYaw = XMScalarModAngle(mYaw + dx * mRotationSpeed);
	mPitch = std::clamp(mPitch + dy * mRotationSpeed, -XM_PIDIV2, XM_PIDIV2);
}

void XM_CALLCONV Camera::Translate(DirectX::FXMVECTOR translation) noexcept
{
	XMVECTOR transform = XMVector3Transform(translation, XMMatrixRotationRollPitchYaw(mPitch, mYaw, 0.0f) * XMMatrixScaling(mTravelSpeed, mTravelSpeed, mTravelSpeed));
	XMStoreFloat3(&mPosition, XMVectorClamp(XMVectorAdd(XMLoadFloat3(&mPosition), transform), XMVectorReplicate(-mMaxRadius), XMVectorReplicate(mMaxRadius)));
}

void Camera::ZoomIn() noexcept
{
	Translate(mForward);
}

void Camera::ZoomOut() noexcept
{
	Translate(XMVectorNegate(mForward));
}
