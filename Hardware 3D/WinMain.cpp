#include <Windows.h>

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	const LPCWSTR pClassName = L"hw3dbutts";

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = DefWindowProc;
	wc.lpszClassName = pClassName;
	wc.style = CS_OWNDC;
	RegisterClassEx(&wc);

	HWND hWnd = CreateWindowEx(0, pClassName, L"Happy Hard Window", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 200, 200, 640, 480, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hWnd, nShowCmd);

	while (true);
	return 0;
}
