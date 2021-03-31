struct VSOut
{
	float3 Color : COLOR;
	float4 PosH : SV_POSITION;
};

cbuffer CBuf
{
	row_major matrix Transform;
};

VSOut main(float2 pos : POSITION, float3 color : COLOR)
{
	VSOut vout;
	vout.PosH = mul(float4(pos.x, pos.y, 0.0f, 1.0f), Transform);
	vout.Color = color;
	return vout;
}
