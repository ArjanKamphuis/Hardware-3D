cbuffer gMaterialCBuf : register(b0)
{
	float3 gMaterialColor;
	float3 gSpecularColor;
	float gSpecularWeight;
	float gSpecularGloss;
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
	const float3 specular = Speculate(gDiffuseColor * gDiffuseIntensity * gSpecularColor, gSpecularWeight, normal, lv.VToL, gCameraPosition, posW, att, gSpecularGloss);

	return float4(saturate((diffuse + gAmbientColor) * gMaterialColor + specular), 1.0f);
}
