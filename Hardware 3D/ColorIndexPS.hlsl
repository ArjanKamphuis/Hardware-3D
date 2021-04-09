cbuffer CBuf
{
	float4 FaceColors[8];
};

float4 main(uint tid : SV_PRIMITIVEID) : SV_TARGET
{
	return FaceColors[(tid / 2) % 8];
}
