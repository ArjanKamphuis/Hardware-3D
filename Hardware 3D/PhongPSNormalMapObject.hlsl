cbuffer gMaterialCBuf : register(b0)
{
	float gSpecularIntensity;
	float gSpecularPower;
	bool gNormalMapEnabled;
	float gMaterialPad;
};

#include "PointLightBuffer.hlsli"
#include "TransformBuffer.hlsli"
#include "LightVectorData.hlsli"
#include "ShaderOperations.hlsli"

Texture2D gTexture;
Texture2D gNormalMap : register(t2);
SamplerState gSampler;

float4 main(float3 posW : POSITION, float3 normal : NORMAL, float2 texC : TEXCOORD) : SV_TARGET
{
	if (gNormalMapEnabled)
	{
		const float3 normalSample = gNormalMap.Sample(gSampler, texC).xyz;
		normal = mul(normalSample * 2.0f - 1.0f, (float3x3)gWorld);
	}
	
	normal = normalize(normal);
	
	const LightVectorData lv = CalculateLightVectorData(gLightPosition, posW);
	const float att = Attenuate(gAttConst, gAttLinear, gAttQuad, lv.DistToL);
	const float3 diffuse = Diffuse(gDiffuseColor, gDiffuseIntensity, att, lv.DirToL, normal);
	const float3 specular = Speculate(gSpecularIntensity.rrr, 1.0f, normal, lv.VToL, gCameraPosition, posW, att, gSpecularPower);

	return float4(saturate((diffuse + gAmbientColor) * gTexture.Sample(gSampler, texC).rgb + specular), 1.0f);
}
