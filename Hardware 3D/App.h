#pragma once

#include <set>
#include "Camera.h"
#include "ChiliTimer.h"
#include "ImguiManager.h"
#include "PointLight.h"
#include "Window.h"

class App
{
public:
	App();
	~App();

	int Go();

private:
	void HandleInput();
	void DoFrame();
	void DoImGui() noexcept;

	void SpawnSimulationWindow() noexcept;
	void SpawnBoxWindowManagerWindow() noexcept;
	void SpawnBoxWindows() noexcept;

private:
	ImguiManager mImgui;
	Window mWnd;
	ChiliTimer mTimer;
	Camera mCamera;
	PointLight mLight;

	static constexpr size_t mNumDrawables = 180;
	std::vector<std::unique_ptr<class Drawable>> mDrawables;
	std::vector<class Box*> mBoxes;
	float mSpeedFactor = 1.0f;

	std::optional<int> mComboBoxIndex;
	std::set<int> mBoxControlIds;
};

