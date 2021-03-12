#include <Windows.h>
#include "WindowsMessageMap.h"

LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	static WindowsMessageMap mm;
	OutputDebugString(mm(msg, lParam, wParam).c_str());

	switch (msg)
	{
	case WM_CLOSE:
		PostQuitMessage(69);
		break;
	}

	return DefWindowProc(hWnd, msg, wParam, lParam);
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	const LPCWSTR pClassName = L"hw3dbutts";

	WNDCLASSEX wc = {};
	wc.cbSize = sizeof(wc);
	wc.hInstance = hInstance;
	wc.lpfnWndProc = WndProc;
	wc.lpszClassName = pClassName;
	wc.style = CS_OWNDC;
	RegisterClassEx(&wc);

	HWND hWnd = CreateWindowEx(0, pClassName, L"Happy Hard Window", WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, 200, 200, 640, 480, nullptr, nullptr, hInstance, nullptr);
	ShowWindow(hWnd, nShowCmd);

	MSG msg = {};
	BOOL bReturn = FALSE;
	while ((bReturn = GetMessage(&msg, nullptr, 0, 0)) != 0)
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return bReturn == -1 ? -1 : static_cast<int>(msg.wParam);
}
