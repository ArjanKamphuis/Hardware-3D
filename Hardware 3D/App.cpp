#include "App.h"

#include <iomanip>
#include <sstream>

App::App()
    : mWnd(800, 300, L"The Donkey Fart Box")
{
}

int App::Go()
{
	MSG msg = {};
	BOOL bReturn = FALSE;
	while ((bReturn = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);

		DoFrame();
	}

	if (bReturn == -1)
		throw CHWND_LAST_EXCEPT();

	return static_cast<int>(msg.wParam);
}

void App::DoFrame()
{
	std::wostringstream oss;
	oss << L"Time elapsed: " << std::setprecision(1) << std::fixed << mTimer.Peek() << "s";
	mWnd.SetTitle(oss.str());
}
