#pragma once

#include "ChiliTimer.h"
#include "ImguiManager.h"
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
	void DoImGui();

private:
	ImguiManager mImgui;
	Window mWnd;
	ChiliTimer mTimer;

	static constexpr size_t mNumDrawables = 180;
	std::vector<std::unique_ptr<class Drawable>> mDrawables;
	float mSpeedFactor = 1.0f;
};

