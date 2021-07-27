struct LightVectorData
{
	float3 VToL;
	float3 DirToL;
	float DistToL;
};

LightVectorData CalculateLightVectorData(const in float3 lightPos, const in float3 posW)
{
	LightVectorData lv;
	lv.VToL = lightPos - posW;
	lv.DistToL = length(lv.VToL);
	lv.DirToL = lv.VToL / lv.DistToL;
	return lv;
}
