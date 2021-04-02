cbuffer ColorBuffer
{
	float4 FaceColors[6];
};

float4 main(uint tid : SV_PRIMITIVEID) : SV_TARGET
{
	return FaceColors[tid / 2];
}
