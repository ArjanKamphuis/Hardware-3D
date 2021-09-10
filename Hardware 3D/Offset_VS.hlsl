#include "TransformBuffer.hlsli"

cbuffer cbOffset : register(b1)
{
	float gOffset;
};

float4 main(float3 pos : POSITION, float3 normal : NORMAL) : SV_POSITION
{
	return mul(float4(pos + normal * gOffset, 1.0f), gWVP);
}
