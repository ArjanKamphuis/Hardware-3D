#pragma once

#include "Camera.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Mesh.h"
#include "PointLight.h"
#include "TestPlane.h"
#include "Window.h"

class App
{
public:
	App(const std::wstring& commandLine = L"");
	~App();

	int Go();

private:
	void HandleInput(float dt);
	void DoFrame(float dt);

	void DoImGui(const Graphics& gfx) noexcept;

private:
	std::wstring mCommandLine;
	ImguiManager mImgui;
	Window mWnd;
	ChiliTimer mTimer;
	Camera mCamera;
	PointLight mLight;

	Model mSponza{ mWnd.Gfx(), "Models/sponza/sponza.obj", 0.05f };
	TestPlane mBluePlane{ mWnd.Gfx(), 6.0f, { 0.3f, 0.3f, 1.0f, 0.0f} };
	TestPlane mRedPlane{ mWnd.Gfx(), 6.0f, { 1.0f, 0.3f, 0.3f, 0.0f} };
	//Model mGobber{ mWnd.Gfx(), "Models/gobber/GoblinX.obj", 6.0f };
	//Model mNano{ mWnd.Gfx(), "Models/nano_textured/nanosuit.obj", 2.0f };
	//Model mWall{ mWnd.Gfx(), "Models/brick_wall/brick_wall.obj", 6.0f };
	//TestPlane mPlane{ mWnd.Gfx(), 6.0f };
};
