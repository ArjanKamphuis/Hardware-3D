cbuffer ObjectBuffer : register(b0)
{
	bool gNormalMapEnabled;
	bool gSpecularMapEnabled;
	bool gHasGloss;
	float gSpecularPowerConst;
	float3 gSpecularColor;
	float gSpecularMapWeight;
}

cbuffer LightCBuf : register(b1)
{
	float3 gCameraPosition;
	float3 gLightPosition;
	float3 gAmbientColor;
	float3 gDiffuseColor;
	float gDiffuseIntensity;
	float gAttConst;
	float gAttLinear;
	float gAttQuad;
};

Texture2D gTexture;
Texture2D gSpecMap;
Texture2D gNormalMap;
SamplerState gSampler;

float3 MapNormal(const in float3 tangent, const in float3 bitangent, const in float3 normal, const in float2 texC, uniform Texture2D normalMap, uniform SamplerState smplr)
{
	const float3x3 tanToTarget = float3x3(tangent, bitangent, normal);
	const float3 normalSample = gNormalMap.Sample(smplr, texC).xyz;
	const float3 tanNormal = normalSample * 2.0f - 1.0f;
	return normalize(mul(tanNormal, tanToTarget));
}

float Attenuate(uniform float attConst, uniform float attLinear, uniform float attQuad, const in float distToL)
{
	return 1.0f / (attConst + attLinear * distToL + attQuad * (distToL * distToL));
}

float3 Diffuse(uniform float3 diffuseColor, uniform float diffuseIntensity, const in float att, const in float3 dirToL, const in float3 normal)
{
	return diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, normal));
}

float3 Speculate(const in float3 specularColor, uniform float specularIntensity, const in float3 normal, const in float3 vToL, const in float3 cameraPosition, const in float3 posW, const in float att, const in float specularPower)
{
	const float rdotl = dot(normalize(reflect(-vToL, normal)), normalize(cameraPosition - posW));
	return att * specularColor * specularIntensity * pow(max(rdotl, 0.0f), specularPower);
}

float4 main(float3 posW : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT, float2 texC : TEXCOORD) : SV_TARGET
{
	normal = normalize(normal);
	if (gNormalMapEnabled)
		normal = MapNormal(normalize(tangent), normalize(bitangent), normal, texC, gNormalMap, gSampler);
	
	const float3 vToL = gLightPosition - posW;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;
	
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

	const float att = Attenuate(gAttConst, gAttLinear, gAttQuad, distToL);
	const float3 diffuse = Diffuse(gDiffuseColor, gDiffuseIntensity, att, dirToL, normal);
	const float3 specularReflected = Speculate(specularReflectionColor, 1.0f, normal, vToL, gCameraPosition, posW, att, specularPower);

	return float4(saturate((diffuse + gAmbientColor) * gTexture.Sample(gSampler, texC).rgb + specularReflected), 1.0f);
}
