Texture2D gTex;
SamplerState gSampler;

float4 main(float2 texC : TEXCOORD) : SV_TARGET
{
	return gTex.Sample(gSampler, texC);
}
