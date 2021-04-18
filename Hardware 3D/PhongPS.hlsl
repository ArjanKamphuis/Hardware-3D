#include "MaterialBuffer.hlsli"

cbuffer LigthCBuf : register(b1)
{
	float3 gLightPosition;
	float3 gAmbientColor;
	float3 gDiffuseColor;
	float gDiffuseIntensity;
	float gAttConst;
	float gAttLinear;
	float gAttQuad;
};

float4 main(float3 posW : POSITION, float3 normal : NORMAL) : SV_TARGET
{
	const float3 vToL = gLightPosition - posW;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;

	const float att = 1.0f / (gAttConst + gAttLinear * distToL + gAttQuad * (distToL * distToL));

	const float3 diffuse = gDiffuseColor * gDiffuseIntensity * att * max(0.0f, dot(dirToL, normal));

	return float4(saturate((diffuse + gAmbientColor) * gMaterialColor), 1.0f);
}