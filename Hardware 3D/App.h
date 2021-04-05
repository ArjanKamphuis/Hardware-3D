#pragma once

#include "ChiliTimer.h"
#include "Window.h"

class App
{
public:
	App();
	~App();

	int Go();

private:
	void DoFrame();

private:
	Window mWnd;
	ChiliTimer mTimer;

	std::vector<std::unique_ptr<class Box>> mBoxes;
};

