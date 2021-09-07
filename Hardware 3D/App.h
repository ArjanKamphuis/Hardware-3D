#pragma once

#include "Camera.h"
#include "ChiliTimer.h"
#include "FrameCommander.h"
#include "ImguiManager.h"
#include "Model.h"
#include "PointLight.h"
#include "ScriptCommander.h"
#include "TestCube.h"
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
	ScriptCommander mScriptCommander;
	ChiliTimer mTimer;
	Camera mCamera;
	FrameCommander mFrameCommander;
	PointLight mLight;

	//Model mSponza{ mWnd.Gfx(), "Models/sponza/sponza.obj", 0.05f };
	//TestPlane mBluePlane{ mWnd.Gfx(), 6.0f, { 0.3f, 0.3f, 1.0f, 0.0f} };
	//TestPlane mRedPlane{ mWnd.Gfx(), 6.0f, { 1.0f, 0.3f, 0.3f, 0.0f} };
	TestCube mCube{ mWnd.Gfx(), 4.0f };
	TestCube mCube2{ mWnd.Gfx(), 4.0f };
	std::unique_ptr<Mesh> mLoadedMesh;
	Model mGobber{ mWnd.Gfx(), "Models/gobber/GoblinX.obj", 6.0f };
	//Model mNano{ mWnd.Gfx(), "Models/nano_textured/nanosuit.obj", 2.0f };
	//Model mWall{ mWnd.Gfx(), "Models/brick_wall/brick_wall.obj", 6.0f };
	//TestPlane mPlane{ mWnd.Gfx(), 6.0f };
};
