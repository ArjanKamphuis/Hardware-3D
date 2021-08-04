#pragma once

#include <DirectXMath.h>
#include <string>
#include "Surface.h"

class TexturePreprocessor
{
public:
	static void RotateXAxis180(const std::wstring& pathIn);
	static void RotateXAxis180(const std::wstring& pathIn, const std::wstring& pathOut);
	static void FlipYAllNormalMapsInObj(const std::wstring& objPath);
	static void FlipYNormalMap(const std::wstring& pathIn, const std::wstring& pathOut);
	static void ValidateNormalMap(const std::wstring& pathIn, float thresholdMin, float thresholdMax);

private:
	template<typename F>
	static void TransformFile(const std::wstring& pathIn, const std::wstring& pathOut, F&& func);
	template<typename F>
	static void TransformSurface(Surface& surface, F&& func);

	static DirectX::XMVECTOR XM_CALLCONV MapColorToNormal(Surface::Color c);
	static Surface::Color XM_CALLCONV MapNormalToColor(DirectX::FXMVECTOR n);
};

template<typename F>
inline void TexturePreprocessor::TransformFile(const std::wstring& pathIn, const std::wstring& pathOut, F&& func)
{
	Surface surface = Surface::FromFile(pathIn);
	TransformSurface(surface, func);
	surface.Save(pathOut);
}

template<typename F>
inline void TexturePreprocessor::TransformSurface(Surface& surface, F&& func)
{
	for (UINT y = 0; y < surface.GetHeight(); y++)
		for (UINT x = 0; x < surface.GetWidth(); x++)
			surface.PutPixel(x, y, MapNormalToColor(func(MapColorToNormal(surface.GetPixel(x, y)), x, y)));
}
