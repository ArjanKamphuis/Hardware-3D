struct VSOut
{
	float2 TexC : TEXCOORD;
	float4 PosH : SV_POSITION;
};

VSOut main(float2 pos : POSITION)
{
	VSOut vso;
	vso.PosH = float4(pos, 0.0f, 1.0f);
	vso.TexC = float2(0.5f * (pos.x + 1.0f), -0.5f * (pos.y - 1.0f));
	return vso;
}
