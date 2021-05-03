#include "App.h"

#include "AssTest.h"
#include "Box.h"
#include "Can.h"
#include "Pyramid.h"
#include "SkinnedBox.h"

#include "Surface.h"
#include "GDIPlusManager.h"

#include "imgui/imgui.h"

GDIPlusManager gdipm;

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
	mNanoBot.Draw(gfx);
	mLight.Draw(gfx);

	if (gfx.IsImguiEnabled())
		DoImGui();
	
	gfx.EndFrame();
}

void App::DoImGui() noexcept
{
	mCamera.SpawnControlWindow();
	mLight.SpawnControlWindow();
}
