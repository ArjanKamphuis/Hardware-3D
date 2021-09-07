cbuffer gMaterialCBuf : register(b0)
{
	float3 gMaterialColor;
	float gMaterialPad;
	float3 gSpecularColor;
	float gSpecularPower;
};

#include "PointLightBuffer.hlsli"
#include "LightVectorData.hlsli"
#include "ShaderOperations.hlsli"

float4 main(float3 posW : POSITION, float3 normal : NORMAL) : SV_TARGET
{
	normal = normalize(normal);
	
	const LightVectorData lv = CalculateLightVectorData(gLightPosition, posW);
	const float att = Attenuate(gAttConst, gAttLinear, gAttQuad, lv.DistToL);
	const float3 diffuse = Diffuse(gDiffuseColor, gDiffuseIntensity, att, lv.DirToL, normal);
	const float3 specular = Speculate(gSpecularColor, 1.0f, normal, lv.VToL, gCameraPosition, posW, att, gSpecularPower);

	return float4(saturate((diffuse + gAmbientColor) * gMaterialColor + specular), 1.0f);
}
