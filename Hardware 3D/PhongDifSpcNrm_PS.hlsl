cbuffer ObjectBuffer : register(b0)
{
	bool gUseGlossAlpha;
	bool gUseSpecularMap;
	float3 gSpecularColor;
	float gSpecularWeight;
	float gSpecularGloss;
	bool gUseNormalMap;
	float gNormalMapWeight;
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
	
	#ifdef MASKED
	clip(texSample.a < 0.1f ? -1 : 1);
	if (dot(normal, posW - gCameraPosition) >= 0.0f)
		normal = -normal;
	#endif
	
	normal = normalize(normal);
	if (gUseNormalMap)
	{
		const float3 mappedNormal = MapNormal(normalize(tangent), normalize(bitangent), normal, texC, gNormalMap, gSampler);
		normal = lerp(normal, mappedNormal, gNormalMapWeight);
	}
	
	const float4 specularSample = gSpecMap.Sample(gSampler, texC);
	const float3 specularReflectionColor = gUseSpecularMap ? specularSample.rgb : gSpecularColor;
	const float specularPower = gUseGlossAlpha ? pow(2.0f, specularSample.a * 13.0f) : gSpecularGloss;
	
	const LightVectorData lv = CalculateLightVectorData(gLightPosition, posW);
	const float att = Attenuate(gAttConst, gAttLinear, gAttQuad, lv.DistToL);
	const float3 diffuse = Diffuse(gDiffuseColor, gDiffuseIntensity, att, lv.DirToL, normal);
	const float3 specularReflected = Speculate(gDiffuseColor * gDiffuseIntensity * specularReflectionColor, gSpecularWeight, normal, lv.VToL, gCameraPosition, posW, att, specularPower);

	return float4(saturate((diffuse + gAmbientColor) * texSample.rgb + specularReflected), 1.0f);
}
