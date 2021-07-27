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

float4 main(float3 posW : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT, float2 texC : TEXCOORD) : SV_TARGET
{
	if (gNormalMapEnabled)
	{
		const float3x3 tanToView = float3x3(normalize(tangent), normalize(bitangent), normalize(normal));
		const float3 normalSample = gNormalMap.Sample(gSampler, texC).xyz;
		
		normal = normalSample * 2.0f - 1.0f;
		normal.y = -normal.y;
		
		normal = mul(normal, tanToView);
	}
	
	normal = normalize(normal);
	
	const float3 vToL = gLightPosition - posW;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;

	const float att = 1.0f / (gAttConst + gAttLinear * distToL + gAttQuad * (distToL * distToL));
	const float3 diffuse = gDiffuseColor * gDiffuseIntensity * att * max(0.0f, dot(dirToL, normal));
	
	const float rdotl = dot(normalize(reflect(-vToL, normal)), normalize(gCameraPosition - posW));
	
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
	
	const float3 specular = att * (gDiffuseColor * gDiffuseIntensity) * pow(max(rdotl, 0.0f), specularPower);
	return float4(saturate((diffuse + gAmbientColor) * gTexture.Sample(gSampler, texC).rgb) + specular * specularReflectionColor, 1.0f);
}
