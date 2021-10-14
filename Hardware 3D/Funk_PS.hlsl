Texture2D gTexture;
SamplerState gSampler;

static const int r = 3;
static const float divisor = (2 * r + 1) * (2 * r + 1);

float4 main(float2 texC : TEXCOORD) : SV_TARGET
{
	uint width, height;
	gTexture.GetDimensions(width, height);
	const float dx = 1.0f / width;
	const float dy = 1.0f / height;
	float4 acc = float4(0.0f, 0.0f, 0.0f, 0.0f);
	
	for (int y = -r; y <= r; y++)
		for (int x = -r; x <= r; x++)
			acc += gTexture.Sample(gSampler, texC + float2(dx * x, dy * y));
	
	return acc / divisor;
}