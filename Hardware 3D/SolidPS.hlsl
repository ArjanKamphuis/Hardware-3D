#include "MaterialBuffer.hlsli"

float4 main() : SV_TARGET
{
	return float4(gMaterialColor, 1.0f);
}
