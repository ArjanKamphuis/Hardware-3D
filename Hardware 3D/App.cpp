#include "App.h"

#include "ChiliUtil.h"
#include "DynamicConstantBuffer.h"
#include "LayoutCodex.h"
#include "Testing.h"

using namespace DirectX;

App::App(const std::wstring& commandLine)
    : mCommandLine(commandLine), mWnd(1280, 720, L"The Donkey Fart Box"), mScriptCommander(ChiliUtil::TokenizeQuoted(commandLine)), mLight(mWnd.Gfx())
{
	TestDynamicConstant();
	mCube.SetPosition({ 4.0f, 0.0f, 0.0f });
	mCube2.SetPosition({ 0.0f, 4.0f, 0.0f });
	//mBluePlane.SetPosition(mCamera.GetPosition());
	//mRedPlane.SetPosition(mCamera.GetPosition());
	//mWall.SetRootTransform(XMMatrixTranslation(-12.0f, 0.0f, 0.0f));
	//mPlane.SetPosition({ 12.0f, 0.0f, 0.0f });
	//mGobber.SetRootTransform(XMMatrixTranslation(0.0f, 0.0f, -4.0f));
	//mNano.SetRootTransform(XMMatrixTranslation(0.0f, -7.0f, 6.0f));
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
	//mNano.Draw(gfx);
	//mGobber.Draw(gfx);
	mLight.Submit(mFrameCommander);
	//mSponza.Draw(gfx);
	//mBluePlane.Draw(gfx);
	//mRedPlane.Draw(gfx);
	mCube.Submit(mFrameCommander);
	mCube2.Submit(mFrameCommander);

	mFrameCommander.Execute(gfx);

	if (gfx.IsImguiEnabled())
		DoImGui(gfx);
	
	gfx.EndFrame();
	mFrameCommander.Reset();
}

void App::DoImGui(const Graphics& gfx) noexcept
{
	mCamera.SpawnControlWindow();
	mLight.SpawnControlWindow();
	//mWall.ShowWindow(gfx, "Wall");
	//mPlane.SpawnControlWindow(gfx);
	//mNano.ShowWindow(gfx, "Nano");
	//mGobber.ShowWindow(gfx, "Gobber");
	//mSponza.ShowWindow(gfx, "Sponza");
	//mBluePlane.SpawnControlWindow(gfx, "Blue Plane");
	//mRedPlane.SpawnControlWindow(gfx, "Red Plane");
	mCube.SpawnControlWindow(gfx, "Cube1");
	mCube2.SpawnControlWindow(gfx, "Cube2");
}
