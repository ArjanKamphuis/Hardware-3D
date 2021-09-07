cbuffer ObjectBuffer : register(b0)
{
	float3 gMaterialColor;
}

float4 main() : SV_TARGET
{
	return float4(gMaterialColor, 1.0f);
}
