#include "App.h"

App::App()
    : mWnd(800, 600, L"The Donkey Fart Box")
{
}

int App::Go()
{
	std::optional<int> exitcode;
	while (!(exitcode = Window::ProcessMessages()))
		DoFrame();
	return *exitcode;
}

void App::DoFrame()
{
	const float c = 0.5f * std::sin(mTimer.Peek()) + 0.5f;
	mWnd.Gfx().ClearBuffer(c, c, 1.0f);
	mWnd.Gfx().DrawTestTriangle(mTimer.Peek(), 0.0f, 0.0f);
	mWnd.Gfx().DrawTestTriangle(-mTimer.Peek(), mWnd.Mouse.GetPosX() / 400.0f - 1.0f, -mWnd.Mouse.GetPosY() / 300.0f + 1.0f);
	mWnd.Gfx().EndFrame();
}
