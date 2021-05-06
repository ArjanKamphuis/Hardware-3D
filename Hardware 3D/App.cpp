#include "App.h"

#include "imgui/imgui.h"

using namespace DirectX;

App::App()
    : mWnd(800, 600, L"The Donkey Fart Box"), mLight(mWnd.Gfx())
{
}

App::~App()
{
}

int App::Go()
{
	std::optional<int> exitcode;
	while (!(exitcode = Window::ProcessMessages()))
	{
		HandleInput();
		DoFrame();
	}
	return *exitcode;
}

void App::HandleInput()
{
	while (const auto e = mWnd.Keyboard.ReadKey())
	{
		if (!e->IsPress())
			return;

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		}
	}

	while (const auto e = mWnd.Mouse.Read())
	{
		switch (e->GetType())
		{
		case Mouse::Event::Type::WheelDown:
			mCamera.ZoomOut();
			break;
		case Mouse::Event::Type::WheelUp:
			mCamera.ZoomIn();
			break;
		}
	}
}

void App::DoFrame()
{
	Graphics& gfx = mWnd.Gfx();

	gfx.BeginFrame(0.07f, 0.0f, 0.12f);
	gfx.SetCamera(mCamera.GetMatrix());

	mLight.SetCameraPosition(mCamera.GetPosition());
	mLight.Bind(gfx);

	mNanoBot.Draw(gfx, XMMatrixRotationRollPitchYaw(mModelParams.Pitch, mModelParams.Yaw, mModelParams.Roll) * XMMatrixTranslationFromVector(XMLoadFloat3(&mModelParams.Position)));
	mLight.Draw(gfx);

	if (gfx.IsImguiEnabled())
		DoImGui();
	
	gfx.EndFrame();
}

void App::DoImGui() noexcept
{
	mCamera.SpawnControlWindow();
	mLight.SpawnControlWindow();
	ShowModelWindow();
}

void App::ShowModelWindow() noexcept
{
	if (ImGui::Begin("Model"))
	{
		ImGui::Text("Orientation");
		ImGui::SliderAngle("Roll", &mModelParams.Roll, -180.0f, 180.0f);
		ImGui::SliderAngle("Pitch", &mModelParams.Pitch, -180.0f, 180.0f);
		ImGui::SliderAngle("Yaw", &mModelParams.Yaw, -180.0f, 180.0f);

		ImGui::Text("Position");
		ImGui::SliderFloat("X", &mModelParams.Position.x, -20.0f, 20.0f);
		ImGui::SliderFloat("Y", &mModelParams.Position.y, -20.0f, 20.0f);
		ImGui::SliderFloat("Z", &mModelParams.Position.z, -20.0f, 20.0f);

		if (ImGui::Button("Reset"))
			mModelParams = {};
	}
	ImGui::End();
}
