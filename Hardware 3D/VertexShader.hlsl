struct VSOut
{
	float3 Color : COLOR;
	float4 PosH : SV_POSITION;
};

cbuffer CBuf
{
	matrix Transform;
};

VSOut main(float3 pos : POSITION, float3 color : COLOR)
{
	VSOut vout;
	vout.PosH = mul(float4(pos, 1.0f), Transform);
	vout.Color = color;
	return vout;
}
