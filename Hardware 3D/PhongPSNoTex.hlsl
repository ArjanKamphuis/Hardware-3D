cbuffer gMaterialCBuf : register(b0)
{
	float3 gMaterialColor;
	float gMaterialPad;
	float3 gSpecularColor;
	float gSpecularPower;
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

float4 main(float3 posW : POSITION, float3 normal : NORMAL) : SV_TARGET
{		
	const float3 vToL = gLightPosition - posW;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;

	const float att = 1.0f / (gAttConst + gAttLinear * distToL + gAttQuad * (distToL * distToL));
	const float3 diffuse = gDiffuseColor * gDiffuseIntensity * att * max(0.0f, dot(dirToL, normal));
	
	const float rdotl = dot(normalize(reflect(-vToL, normal)), normalize(gCameraPosition - posW));
	const float3 specular = att * (gDiffuseColor * gDiffuseIntensity) * gSpecularColor * pow(max(rdotl, 0.0f), gSpecularPower);

	return float4(saturate((diffuse + gAmbientColor) * gMaterialColor + specular), 1.0f);
}
