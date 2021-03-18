#pragma once

#include <unordered_map>
#include <Windows.h>

class WindowsMessageMap
{
public:
	WindowsMessageMap() noexcept;
	std::wstring operator()(DWORD msg, LPARAM lParam, WPARAM wParam) const noexcept;

private:
	std::unordered_map<DWORD, std::wstring> mMap;
};

