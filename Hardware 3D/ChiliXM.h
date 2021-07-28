#pragma once
#include <DirectXMath.h>

namespace ChiliXM
{
	DirectX::XMVECTOR XM_CALLCONV ExtractEulerAngles(DirectX::FXMMATRIX matrix);
	DirectX::XMVECTOR XM_CALLCONV ExtractTranslation(DirectX::FXMMATRIX matrix);
}
