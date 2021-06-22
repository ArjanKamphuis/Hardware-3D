#include "TransformBuffer.hlsli"

struct VSOut
{
	float3 PosW : POSITION;
	float3 Normal : NORMAL;
	float3 Tangent : TANGENT;
	float3 Bitangent : BITANGENT;
	float2 TexC : TEXCOORD;
	float4 PosH : SV_POSITION;
};

VSOut main(float3 position : POSITION, float3 normal : NORMAL, float3 tangent : TANGENT, float3 bitangent : BITANGENT, float2 texC : TEXCOORD)
{
	VSOut vout;
	vout.PosW = mul(float4(position, 1.0f), gWorld).xyz;
	vout.Normal = mul(normal, (float3x3)gWorld);
	vout.Tangent = mul(tangent, (float3x3)gWorld);
	vout.Bitangent = mul(bitangent, (float3x3)gWorld);
	vout.TexC = texC;
	vout.PosH = mul(float4(position, 1.0f), gWVP);
	return vout;
}