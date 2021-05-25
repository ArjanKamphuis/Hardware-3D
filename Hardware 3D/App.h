#pragma once

#include "Camera.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "Mesh.h"
#include "PointLight.h"
#include "Window.h"

class App
{
public:
	App();
	~App();

	int Go();

private:
	void HandleInput(float dt);
	void DoFrame(float dt);

	void DoImGui() noexcept;

private:
	ImguiManager mImgui;
	Window mWnd;
	ChiliTimer mTimer;
	Camera mCamera;
	PointLight mLight;

	Model mNanoBot{ mWnd.Gfx(), "Models/nano_hierarchy.gltf" };
};

