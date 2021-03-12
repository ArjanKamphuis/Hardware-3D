#pragma once

#include <unordered_map>
#include <Windows.h>

class WindowsMessageMap
{
public:
	WindowsMessageMap();
	std::wstring operator()(DWORD msg, LPARAM lParam, WPARAM wParam) const;

private:
	std::unordered_map<DWORD, std::wstring> mMap;
};

