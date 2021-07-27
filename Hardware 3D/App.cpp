#include "App.h"

#include "GDIPlusManager.h"
GDIPlusManager gdipm;

#include <shellapi.h>
#include "NormalMapTwerker.h"

using namespace DirectX;

App::App(const std::wstring& commandLine)
    : mCommandLine(commandLine), mWnd(1280, 720, L"The Donkey Fart Box"), mLight(mWnd.Gfx())
{
	if (mCommandLine != L"")
	{
		int nArgs;
		const LPWSTR* pArgs = CommandLineToArgvW(mCommandLine.c_str(), &nArgs);
		if (nArgs >= 3 /*&& pArgs[0] == L"--ntwerk-rotx180"*/)
		{
			NormalMapTwerker::RotateXAxis180(pArgs[1], pArgs[2]);
			throw std::runtime_error("Normal map processed successfully. Just kidding about that whole runtime error thing.");
		}
	}

	mWall.SetRootTransform(XMMatrixTranslation(-12.0f, 0.0f, 0.0f));
	mPlane.SetPosition({ 12.0f, 0.0f, 0.0f });
	mGobber.SetRootTransform(XMMatrixTranslation(0.0f, 0.0f, -4.0f));
	mNano.SetRootTransform(XMMatrixTranslation(0.0f, -7.0f, 6.0f));
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
	
	mWall.Draw(gfx);
	mPlane.Draw(gfx);
	mNano.Draw(gfx);
	mGobber.Draw(gfx);
	mLight.Draw(gfx);

	if (gfx.IsImguiEnabled())
		DoImGui(gfx);
	
	gfx.EndFrame();
}

void App::DoImGui(const Graphics& gfx) noexcept
{
	mCamera.SpawnControlWindow();
	mLight.SpawnControlWindow();
	mWall.ShowWindow(gfx, "Wall");
	mPlane.SpawnControlWindow(gfx);
	mNano.ShowWindow(gfx, "Nano");
	mGobber.ShowWindow(gfx, "Gobber");
}
