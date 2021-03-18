#include "App.h"

#include <iomanip>
#include <sstream>

App::App()
    : mWnd(800, 300, L"The Donkey Fart Box")
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
	std::wostringstream oss;
	oss << L"Time elapsed: " << std::setprecision(1) << std::fixed << mTimer.Peek() << "s";
	mWnd.SetTitle(oss.str());
	Sleep(1);
}
