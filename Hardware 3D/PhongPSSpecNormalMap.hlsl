cbuffer ObjectBuffer : register(b0)
{
	bool gNormalMapEnabled;
	bool gSpecularMapEnabled;
	bool gHasGloss;
	float gSpecularPowerConst;
	float3 gSpecularColor;
	float gSpecularMapWeight;
}

#include "PointLightBuffer.hlsli"
#include "LightVectorData.hlsli"
#include "ShaderOperations.hlsli"

Texture2D gTexture;
Texture2D gSpecMap;
Texture2D gNormalMap;
SamplerState gSampler;

float4 main(float3 posW : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT, float2 texC : TEXCOORD) : SV_TARGET
{
	const float4 texSample = gTexture.Sample(gSampler, texC);
	clip(texSample.a < 0.1f ? -1 : 1);
	
	normal = normalize(normal);
	if (gNormalMapEnabled)
		normal = MapNormal(normalize(tangent), normalize(bitangent), normal, texC, gNormalMap, gSampler);
	
	const LightVectorData lv = CalculateLightVectorData(gLightPosition, posW);
	float3 specularReflectionColor;
	float specularPower = gSpecularPowerConst;
	
	if (gSpecularMapEnabled)
	{
		const float4 specularSample = gSpecMap.Sample(gSampler, texC);
		specularReflectionColor = specularSample.rgb * gSpecularMapWeight;
		if (gHasGloss)
			specularPower = pow(2.0f, specularSample.a * 13.0f);
	}
	else
		specularReflectionColor = gSpecularColor;	

	const float att = Attenuate(gAttConst, gAttLinear, gAttQuad, lv.DistToL);
	const float3 diffuse = Diffuse(gDiffuseColor, gDiffuseIntensity, att, lv.DirToL, normal);
	const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, normal, lv.VToL, gCameraPosition, posW, att, specularPower);

	return float4(saturate((diffuse + gAmbientColor) * texSample.rgb + specularReflected), texSample.a);
}
