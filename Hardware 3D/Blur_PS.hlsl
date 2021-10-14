Texture2D gTexture;
SamplerState gSampler;

cbuffer cbKernel
{
	uint gNumTaps;
	float gCoefficients[31];
};

cbuffer cbControl
{
	bool gHorizontal;
};

float4 main(float2 texC : TEXCOORD) : SV_TARGET
{
	uint width, height;
	gTexture.GetDimensions(width, height);
	float dx = 0.0f, dy = 0.0f;
	
	if (gHorizontal)
		dx = 1.0f / width;
	else
		dy = 1.0f / height;
	
	const int r = gNumTaps / 2;
	
	float4 acc = float4(0.0f, 0.0f, 0.0f, 0.0f);
	for (int i = -r; i <= r; i++)
		acc += gTexture.Sample(gSampler, texC + float2(dx * i, dy * i)) * gCoefficients[i + r];
	
	return acc;
}
