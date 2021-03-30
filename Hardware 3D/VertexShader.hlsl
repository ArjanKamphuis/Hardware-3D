struct VSOut
{
	float3 Color : COLOR;
	float4 PosH : SV_POSITION;
};

VSOut main(float2 pos : POSITION, float3 color : COLOR)
{
	VSOut vout;
	vout.PosH = float4(pos.x, pos.y, 0.0f, 1.0f);
	vout.Color = color;
	return vout;
}
