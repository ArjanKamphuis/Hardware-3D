#include "App.h"

#include <iomanip>
#include <sstream>

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
	mWnd.Gfx().EndFrame();
}
