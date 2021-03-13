#pragma once

#include "ChiliWin.h"

class Window
{
private:
	class WindowClass
	{
	public:
		static const wchar_t* GetName() noexcept;
		static HINSTANCE GetInstance() noexcept;

	private:
		WindowClass() noexcept;
		WindowClass(const WindowClass&) = delete;
		WindowClass& operator=(const WindowClass&) = delete;
		~WindowClass();

	private:
		static constexpr const wchar_t* mWndClassName = L"Chili Direct3D Enginge Window";
		static WindowClass mWndClass;
		HINSTANCE mhInstance;
	};

public:
	Window(int width, int height, const wchar_t* name) noexcept;
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();

private:
	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

private:
	int mWidth;
	int mHeight;
	HWND mhWnd;
};

