cbuffer ObjectBuffer : register(b0)
{
	bool gUseGlossAlpha;
	bool gUseSpecularMap;
	float3 gSpecularColor;
	float gSpecularWeight;
	float gSpecularGloss;
	float2 gMatPad;
}

#include "PointLightBuffer.hlsli"
#include "LightVectorData.hlsli"
#include "ShaderOperations.hlsli"

Texture2D gTexture;
Texture2D gSpecMap;
SamplerState gSampler;

float4 main(float3 posW : POSITION, float3 normal : NORMAL, float2 texC : TEXCOORD) : SV_TARGET
{
	normal = normalize(normal);
	
	const LightVectorData lv = CalculateLightVectorData(gLightPosition, posW);
	
	const float4 specularSample = gSpecMap.Sample(gSampler, texC);
	const float3 specularReflectionColor = gUseSpecularMap ? specularSample.rgb : gSpecularColor;
	const float specularPower = gUseGlossAlpha ? pow(2.0f, specularSample.a * 13.0f) : gSpecularGloss;

	const float att = Attenuate(gAttConst, gAttLinear, gAttQuad, lv.DistToL);
	const float3 diffuse = Diffuse(gDiffuseColor, gDiffuseIntensity, att, lv.DirToL, normal);
	const float3 specularReflected = Speculate(gDiffuseColor * gDiffuseIntensity * specularReflectionColor, 1.0f, normal, lv.VToL, gCameraPosition, posW, att, specularPower);

	return float4(saturate((diffuse + gAmbientColor) * gTexture.Sample(gSampler, texC).rgb + specularReflected), 1.0f);
}
