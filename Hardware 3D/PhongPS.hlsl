cbuffer LigthCBuf
{
	float3 gLightPos;
};

static const float3 gMaterialColor = { 0.7f, 0.7f, 0.9f };
static const float3 gAmbient = { 0.05f, 0.05f, 0.05f };
static const float3 gDiffuseColor = { 1.0f, 1.0f, 1.0f };
static const float gDiffuseIntensity = 1.0f;
static const float gAttConst = 1.0f;
static const float gAttLinear = 0.045f;
static const float gAttQuad = 0.0075f;

float4 main(float3 posW : POSITION, float3 normal : NORMAL) : SV_TARGET
{
	const float3 vToL = gLightPos - posW;
	const float distToL = length(vToL);
	const float3 dirToL = vToL / distToL;

	const float att = 1.0f / (gAttConst + gAttLinear * distToL + gAttQuad * (distToL * distToL));

	const float3 diffuse = gDiffuseColor * gDiffuseIntensity * att * max(0.0f, dot(dirToL, normal));

	return float4(saturate((diffuse + gAmbient) * gMaterialColor), 1.0f);
}