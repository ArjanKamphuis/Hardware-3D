#pragma once

#include "ChiliWin.h"
#include <memory>
#include <optional>
#include "ChiliException.h"
#include "Graphics.h"
#include "Keyboard.h"
#include "Mouse.h"

class Window
{
public:
	class Exception : public ChiliException
	{
		using ChiliException::ChiliException;
	public:
		static std::wstring TranslateErrorCode(HRESULT hr) noexcept;
	};

	class HrException : public Exception
	{
	public:
		HrException(int line, const wchar_t* file, HRESULT hr) noexcept;
		const wchar_t* GetType() const noexcept override;
		HRESULT GetErrorCode() const noexcept;
		std::wstring GetErrorDescription() const noexcept;

	protected:
		void GenerateMessage() const noexcept override;

	private:
		HRESULT mHR;
	};

	class NoGfxException : public Exception
	{
	public:
		using Exception::Exception;
		const wchar_t* GetType() const noexcept override;
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

	void SetTitle(const std::wstring& title);
	static std::optional<int> ProcessMessages() noexcept;

	Graphics& Gfx() const;

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

	std::unique_ptr<Graphics> mGfx;
};

#define CHWND_EXCEPT(hr) Window::HrException(__LINE__, __FILEW__, hr)
#define CHWND_LAST_EXCEPT() Window::HrException(__LINE__, __FILEW__, GetLastError())
#define CHWND_NOGFX_EXCEPT() Window::NoGfxException(__LINE__, __FILEW__)
