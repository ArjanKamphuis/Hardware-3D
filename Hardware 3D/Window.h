#pragma once

#include "ChiliWin.h"
#include "ChiliException.h"
#include "Keyboard.h"
#include "Mouse.h"

class Window
{
public:
	class Exception : public ChiliException
	{
	public:
		Exception(int line, const wchar_t* file, HRESULT hr) noexcept;
		const wchar_t* GetType() const noexcept override;
		static std::wstring TranslateErrorCode(HRESULT hr) noexcept;
		HRESULT GetErrorCode() const noexcept;
		std::wstring GetErrorString() const noexcept;

	protected:
		void GenerateMessage() const noexcept override;

	private:
		HRESULT mHR;
	};

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
		static constexpr const wchar_t* mWndClassName = L"Chili Direct3D Engine Window";
		static WindowClass mWndClass;
		HINSTANCE mhInstance;
	};

public:
	Window(int width, int height, const wchar_t* name);
	Window(const Window&) = delete;
	Window& operator=(const Window&) = delete;
	~Window();

private:
	void AdjustAndCenterWindow();

	static LRESULT CALLBACK HandleMsgSetup(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	static LRESULT CALLBACK HandleMsgThunk(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;
	LRESULT HandleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) noexcept;

public:
	Keyboard Keyboard;
	Mouse Mouse;

private:
	int mWidth;
	int mHeight;
	HWND mhWnd;
};

#define CHWND_EXCEPT(hr) Window::Exception(__LINE__, __FILEW__, hr)
#define CHWND_LAST_EXCEPT() Window::Exception(__LINE__, __FILEW__, GetLastError())
