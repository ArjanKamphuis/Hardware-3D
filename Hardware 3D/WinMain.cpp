#include "Window.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	Window wnd(800, 300, L"Donkey Fart Box");

	MSG msg = {};
	BOOL bReturn = FALSE;
	while ((bReturn = GetMessage(&msg, nullptr, 0, 0)) > 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return bReturn == -1 ? -1 : static_cast<int>(msg.wParam);
}
