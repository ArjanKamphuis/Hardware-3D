#include "MaterialBuffer.hlsli"

Texture2D gTex;
SamplerState gSampler;

float4 main(float2 texC : TEXCOORD) : SV_TARGET
{
	return gTex.Sample(gSampler, texC) * float4(gMaterialColor, 1.0f);
}
