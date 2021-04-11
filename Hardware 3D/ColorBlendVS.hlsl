cbuffer Cbuf
{
	matrix Transform;
};

struct VSOut
{
	float4 Color : COLOR;
	float4 PosH : SV_POSITION;
};

VSOut main(float3 pos : POSITION, float4 color : COLOR)
{
	VSOut vout;
	vout.PosH = mul(float4(pos, 1.0f), Transform);
	vout.Color = color;
	return vout;
}