#include "Window.h"

#include "App.h"
#include "ChiliUtil.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	try
	{
		return App{ GetCommandLineW() }.Go();
	}
	catch (const ChiliException& e)
	{
		MessageBox(nullptr, e.GetFullMessage(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		MessageBox(nullptr, ChiliUtil::ToWide(e.what()).c_str(), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}
