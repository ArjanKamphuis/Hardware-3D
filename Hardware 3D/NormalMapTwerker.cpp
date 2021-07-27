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
		*pCurrent = VectorToColor(XMVector3Transform(ColorToVector(*pCurrent), rotation));

	sIn.Save(pathOut);
}

DirectX::XMVECTOR XM_CALLCONV NormalMapTwerker::ColorToVector(Surface::Color c)
{
	return XMVectorSubtract(XMVectorMultiply(XMLoadColor(&c), XMVectorReplicate(2.0f)), XMVectorReplicate(1.0f));
}

Color XM_CALLCONV NormalMapTwerker::VectorToColor(DirectX::FXMVECTOR n)
{
	XMCOLOR xmc;
	XMStoreColor(&xmc, XMVectorDivide(XMVectorAdd(n, XMVectorReplicate(1.0f)), XMVectorReplicate(2.0f)));
	return xmc;
}
