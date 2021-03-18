#include "Window.h"

#include <sstream>

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

			static int i = 0;
			while (!wnd.Mouse.IsEmpty())
			{
				const Mouse::Event e = wnd.Mouse.Read();
				switch (e.GetType())
				{
				case Mouse::Event::Type::WheelUp:
					i++;
					{
						std::wostringstream oss;
						oss << L"Up: " << i;
						wnd.SetTitle(oss.str());
					}
					break;
				case Mouse::Event::Type::WheelDown:
					i--;
					{
						std::wstringstream oss;
						oss << L"Down: " << i;
						wnd.SetTitle(oss.str());
					}
					break;
				}
			}
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
