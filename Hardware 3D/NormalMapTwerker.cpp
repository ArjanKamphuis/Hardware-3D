#include "NormalMapTwerker.h"

using namespace DirectX;
using namespace DirectX::PackedVector;
using Color = Surface::Color;

void NormalMapTwerker::RotateXAxis180(const std::wstring& pathIn)
{
	return RotateXAxis180(pathIn, pathIn);
}

void NormalMapTwerker::RotateXAxis180(const std::wstring& pathIn, const std::wstring& pathOut)
{
	const XMMATRIX rotation = XMMatrixRotationX(XM_PI);
	Surface sIn = Surface::FromFile(pathIn);

	const UINT nPixels = sIn.GetWidth() * sIn.GetHeight();
	const auto pBegin = sIn.GetBufferPtr();
	const auto pEnd = sIn.GetBufferPtrConst() + nPixels;

	for (auto pCurrent = pBegin; pCurrent < pEnd; ++pCurrent)
		*pCurrent = MapNormalToColor(XMVector3Transform(MapColorToNormal(*pCurrent), rotation));

	sIn.Save(pathOut);
}

// Map Color[0,1] to Normal[-1,-1]
DirectX::XMVECTOR XM_CALLCONV NormalMapTwerker::MapColorToNormal(Surface::Color c)
{
	return XMVectorSubtract(XMVectorMultiply(XMLoadColor(&c), XMVectorReplicate(2.0f)), XMVectorReplicate(1.0f));
}

//Map Normal[-1,1] to Color[0,1]
Color XM_CALLCONV NormalMapTwerker::MapNormalToColor(DirectX::FXMVECTOR n)
{
	XMCOLOR xmc;
	XMStoreColor(&xmc, XMVectorDivide(XMVectorAdd(n, XMVectorReplicate(1.0f)), XMVectorReplicate(2.0f)));
	return xmc;
}
