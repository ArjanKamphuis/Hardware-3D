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

private:
	template<typename F>
	static void TransformFile(const std::wstring& pathIn, const std::wstring& pathOut, F&& func);
	static DirectX::XMVECTOR XM_CALLCONV MapColorToNormal(Surface::Color c);
	static Surface::Color XM_CALLCONV MapNormalToColor(DirectX::FXMVECTOR n);
};

template<typename F>
inline void TexturePreprocessor::TransformFile(const std::wstring& pathIn, const std::wstring& pathOut, F&& func)
{
	Surface surface = Surface::FromFile(pathIn);

	const UINT nPixels = surface.GetWidth() * surface.GetHeight();
	const auto pBegin = surface.GetBufferPtr();
	const auto pEnd = surface.GetBufferPtrConst() + nPixels;

	for (auto pCurrent = pBegin; pCurrent < pEnd; ++pCurrent)
		*pCurrent = MapNormalToColor(func(MapColorToNormal(*pCurrent)));

	surface.Save(pathOut);
}
