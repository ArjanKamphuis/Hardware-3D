cbuffer ObjectBuffer : register(b0)
{
	float4 gMaterialColor;
}

float4 main() : SV_TARGET
{
	return gMaterialColor;
}
