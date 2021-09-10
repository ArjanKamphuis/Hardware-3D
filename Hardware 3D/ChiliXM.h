#pragma once
#include <DirectXMath.h>

namespace ChiliXM
{
	DirectX::XMVECTOR XM_CALLCONV ExtractEulerAngles(DirectX::FXMMATRIX matrix);
	DirectX::XMVECTOR XM_CALLCONV ExtractTranslation(DirectX::FXMMATRIX matrix);
	DirectX::XMMATRIX XM_CALLCONV ScaleTranslation(DirectX::XMMATRIX matrix, float scale);
}
