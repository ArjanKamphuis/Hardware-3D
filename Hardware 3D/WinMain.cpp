#include "Window.h"

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
	try
	{
		Window wnd(800, 300, L"Donkey Fart Box");

		MSG msg = {};
		BOOL bReturn = FALSE;
		while ((bReturn = GetMessage(&msg, nullptr, 0, 0)) > 0)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);

			if (wnd.Keyboard.KeyIsPressed(VK_MENU))
				MessageBox(nullptr, L"Something Happon!", L"Alt Key Was Pressed", MB_OK | MB_ICONEXCLAMATION);
		}

		if (bReturn == -1)
			throw CHWND_LAST_EXCEPT();

		return static_cast<int>(msg.wParam);
	}
	catch (const ChiliException& e)
	{
		MessageBox(nullptr, e.GetFullMessage(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
	}
	catch (const std::exception& e)
	{
		const std::string message = std::string(e.what());
		MessageBox(nullptr, std::wstring(message.begin(), message.end()).c_str(), L"Standard Exception", MB_OK | MB_ICONEXCLAMATION);
	}
	catch (...)
	{
		MessageBox(nullptr, L"No details available", L"Unknown Exception", MB_OK | MB_ICONEXCLAMATION);
	}

	return -1;
}
