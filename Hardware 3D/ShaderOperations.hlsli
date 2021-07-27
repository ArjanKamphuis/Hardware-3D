float3 MapNormal(const in float3 tangent, const in float3 bitangent, const in float3 normal, const in float2 texC, uniform Texture2D normalMap, uniform SamplerState smplr)
{
	const float3x3 tanToTarget = float3x3(tangent, bitangent, normal);
	const float3 normalSample = normalMap.Sample(smplr, texC).xyz;
	const float3 tanNormal = normalSample * 2.0f - 1.0f;
	return normalize(mul(tanNormal, tanToTarget));
}

float Attenuate(uniform float attConst, uniform float attLinear, uniform float attQuad, const in float distToL)
{
	return 1.0f / (attConst + attLinear * distToL + attQuad * (distToL * distToL));
}

float3 Diffuse(uniform float3 diffuseColor, uniform float diffuseIntensity, const in float att, const in float3 dirToL, const in float3 normal)
{
	return diffuseColor * diffuseIntensity * att * max(0.0f, dot(dirToL, normal));
}

float3 Speculate(const in float3 specularColor, uniform float specularIntensity, const in float3 normal, const in float3 vToL, const in float3 cameraPosition, const in float3 posW, const in float att, const in float specularPower)
{
	const float rdotl = dot(normalize(reflect(-vToL, normal)), normalize(cameraPosition - posW));
	return att * specularColor * specularIntensity * pow(max(rdotl, 0.0f), specularPower);
}
