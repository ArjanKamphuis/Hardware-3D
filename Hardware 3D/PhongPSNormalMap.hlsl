cbuffer gMaterialCBuf : register(b0)
{
	float gSpecularIntensity;
	float gSpecularPower;
	bool gNormalMapEnabled;
	float gMaterialPad;
};

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
Texture2D gNormalMap : register(t2);
SamplerState gSampler;

float4 main(float3 posW : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT, float2 texC : TEXCOORD) : SV_TARGET
{
	if (gNormalMapEnabled)
	{	
		const float3x3 tanToView = float3x3(normalize(tangent), normalize(bitangent), normalize(normal));
		const float4 normalSample = gNormalMap.Sample(gSampler, texC);
		
		normal.x = normalSample.x * 2.0f - 1.0f;
		normal.y = -normalSample.y * 2.0f + 1.0f;
		normal.z = normalSample.z;
		
		normal = mul(normal, tanToView);
	}
	
	const float3 vToL = gLightPosition - posW;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;

	const float att = 1.0f / (gAttConst + gAttLinear * distToL + gAttQuad * (distToL * distToL));
	const float3 diffuse = gDiffuseColor * gDiffuseIntensity * att * max(0.0f, dot(dirToL, normal));
	
	const float rdotl = dot(normalize(reflect(-vToL, normal)), normalize(gCameraPosition - posW));
	const float3 specular = att * (gDiffuseColor * gDiffuseIntensity) * gSpecularIntensity * pow(max(rdotl, 0.0f), gSpecularPower);

	return float4(saturate((diffuse + gAmbientColor) * gTexture.Sample(gSampler, texC).rgb + specular), 1.0f);
}
