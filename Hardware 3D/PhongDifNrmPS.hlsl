cbuffer gMaterialCBuf : register(b0)
{
	float3 gSpecularColor;
	float gSpecularWeight;
	float gSpecularGloss;
	bool gUseNormalMap;
	float gNormalMapWeight;
	float gMatPad;
};

#include "PointLightBuffer.hlsli"
#include "LightVectorData.hlsli"
#include "ShaderOperations.hlsli"

Texture2D gTexture;
Texture2D gNormalMap : register(t2);
SamplerState gSampler;

float4 main(float3 posW : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT, float2 texC : TEXCOORD) : SV_TARGET
{
	normal = normalize(normal);
	if (gUseNormalMap)
	{
		const float3 mappedNormal = MapNormal(normalize(tangent), normalize(bitangent), normal, texC, gNormalMap, gSampler);
		normal = lerp(normal, mappedNormal, gNormalMapWeight);
	}
	
	const LightVectorData lv = CalculateLightVectorData(gLightPosition, posW);
	const float att = Attenuate(gAttConst, gAttLinear, gAttQuad, lv.DistToL);
	const float3 diffuse = Diffuse(gDiffuseColor, gDiffuseIntensity, att, lv.DirToL, normal);
	const float3 specular = Speculate(gDiffuseColor * gDiffuseIntensity * gSpecularColor, gSpecularWeight, normal, lv.VToL, gCameraPosition, posW, att, gSpecularGloss);

	return float4(saturate((diffuse + gAmbientColor) * gTexture.Sample(gSampler, texC).rgb + specular), 1.0f);
}
