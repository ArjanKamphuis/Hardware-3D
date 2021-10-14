Texture2D gTexture;
SamplerState gSampler;

float4 main(float2 texC : TEXCOORD) : SV_TARGET
{
	return 1.0f - gTexture.Sample(gSampler, texC);
}