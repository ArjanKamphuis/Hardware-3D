Texture2D gTexture;
SamplerState gSampler;

static const int r = 12;
static const float divisor = (2 * r + 1) * (2 * r + 1);

float4 main(float2 texC : TEXCOORD) : SV_TARGET
{
	uint width, height;
	gTexture.GetDimensions(width, height);
	const float dx = 1.0f / width;
	const float dy = 1.0f / height;
	
	float accAlpha = 0.0f;
	float3 maxColor = float3(0.0f, 0.0f, 0.0f);
	
	for (int y = -r; y <= r; y++)
	{
		for (int x = -r; x <= r; x++)
		{
			const float4 s = gTexture.Sample(gSampler, texC + float2(dx * x, dy * y));
			accAlpha += s.a;
			maxColor = max(maxColor, s.rgb);
		}
	}
	
	return float4(maxColor, accAlpha / divisor);
}
