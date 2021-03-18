#pragma once

#include "ChiliTimer.h"
#include "Window.h"

class App
{
public:
	App();
	int Go();

private:
	void DoFrame();

private:
	Window mWnd;
	ChiliTimer mTimer;
};

