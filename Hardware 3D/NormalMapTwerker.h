#pragma once

#include <DirectXMath.h>
#include <string>
#include "Surface.h"

class NormalMapTwerker
{
public:
	static void RotateXAxis180(const std::wstring& pathIn);
	static void RotateXAxis180(const std::wstring& pathIn, const std::wstring& pathOut);
private:
	static DirectX::XMVECTOR XM_CALLCONV MapColorToNormal(Surface::Color c);
	static Surface::Color XM_CALLCONV MapNormalToColor(DirectX::FXMVECTOR n);
};
