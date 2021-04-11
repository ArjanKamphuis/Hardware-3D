cbuffer Cbuf
{
	matrix gTransform;
};

struct VSOut
{
	float2 TexC : TEXCOORD;
	float4 PosH : SV_POSITION;
};

VSOut main(float3 pos : POSITION, float2 texC : TEXCOORD)
{
	VSOut vout;
	vout.PosH = mul(float4(pos, 1.0f), gTransform);
	vout.TexC = texC;
	return vout;
}
