cbuffer ObjectCBuf : register(b0)
{
	float gSpecularIntensity;
	float gSpecularPower;
};

cbuffer LigthCBuf : register(b1)
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
SamplerState gSampler;

float4 main(float3 posW : POSITION, float3 normal : NORMAL, float2 texC : TEXCOORD) : SV_TARGET
{
	const float3 vToL = gLightPosition - posW;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;

	const float att = 1.0f / (gAttConst + gAttLinear * distToL + gAttQuad * (distToL * distToL));
	const float3 diffuse = gDiffuseColor * gDiffuseIntensity * att * max(0.0f, dot(dirToL, normal));
	
	const float rdotl = dot(normalize(reflect(-vToL, normal)), normalize(gCameraPosition - posW));
	const float3 specular = att * (gDiffuseColor * gDiffuseIntensity) * gSpecularIntensity * pow(max(rdotl, 0.0f), gSpecularPower);

	return float4(saturate(diffuse + specular + gAmbientColor), 1.0f) * gTexture.Sample(gSampler, texC);
}