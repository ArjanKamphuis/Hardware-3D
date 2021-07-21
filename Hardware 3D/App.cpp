#include "App.h"

#include "GDIPlusManager.h"
GDIPlusManager gdipm;

using namespace DirectX;

App::App()
    : mWnd(1280, 720, L"The Donkey Fart Box"), mLight(mWnd.Gfx())
{
	//mWall.SetRootTransform(XMMatrixTranslation(-1.5f, 0.0f, 0.0f));
	//mCube.SetRootTransform(XMMatrixTranslation(0.0f, 0.0f, 5.0f));
	//mPlane.SetPosition({ 1.5f, 0.0f, 0.0f });
}

App::~App()
{
}

int App::Go()
{
	std::optional<int> exitcode;
	while (!(exitcode = Window::ProcessMessages()))
	{
		const float dt = mTimer.Mark();
		HandleInput(dt);
		DoFrame(dt);
	}
	return *exitcode;
}

void App::HandleInput(float dt)
{
	while (const auto e = mWnd.Keyboard.ReadKey())
	{
		if (!e->IsPress())
			continue;

		switch (e->GetCode())
		{
		case VK_ESCAPE:
			PostQuitMessage(0);
			break;
		case VK_INSERT:
			if (mWnd.CursorEnabled())
			{
				mWnd.DisableCursor();
				mWnd.Mouse.EnableRaw();
			}
			else
			{
				mWnd.EnableCursor();
				mWnd.Mouse.DisableRaw();
			}
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

	if (!mWnd.CursorEnabled())
	{
		if (mWnd.Keyboard.KeyIsPressed('W'))
			mCamera.Translate({ 0.0f, 0.0f, dt });
		if (mWnd.Keyboard.KeyIsPressed('A'))
			mCamera.Translate({ -dt, 0.0f, 0.0f });
		if (mWnd.Keyboard.KeyIsPressed('S'))
			mCamera.Translate({ 0.0f, 0.0f, -dt });
		if (mWnd.Keyboard.KeyIsPressed('D'))
			mCamera.Translate({ dt, 0.0f, 0.0f });
		if (mWnd.Keyboard.KeyIsPressed('E'))
			mCamera.Translate({ 0.0f, dt, 0.0f });
		if (mWnd.Keyboard.KeyIsPressed('Q'))
			mCamera.Translate({ 0.0f, -dt, 0.0f });
	}

	while (const auto delta = mWnd.Mouse.ReadRawDelta())
	{
		if (!mWnd.CursorEnabled())
			mCamera.Rotate(static_cast<float>(delta->X), static_cast<float>(delta->Y));
	}
}

void App::DoFrame(float dt)
{
	Graphics& gfx = mWnd.Gfx();

	gfx.BeginFrame(0.07f, 0.0f, 0.12f);
	gfx.SetCamera(mCamera.GetMatrix());

	mLight.SetCameraPosition(mCamera.GetPosition());
	mLight.Bind(gfx);
	
	//mWall.Draw(gfx);
	//mPlane.Draw(gfx);
	//mCube.Draw(gfx);
	mGobber.Draw(gfx);
	mLight.Draw(gfx);

	if (gfx.IsImguiEnabled())
		DoImGui();
	
	gfx.EndFrame();
}

void App::DoImGui() noexcept
{
	mCamera.SpawnControlWindow();
	mLight.SpawnControlWindow();
	//mWall.ShowWindow("Wall");
	//mPlane.SpawnControlWindow(mWnd.Gfx());
	//mCube.ShowWindow("Cube");
	mGobber.ShowWindow("Gobber");
}
