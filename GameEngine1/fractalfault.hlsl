struct Fault
{
	float offset;
	float gradient;
	bool gt;
};

RWTexture2D<float> tex;
StructuredBuffer<Fault> faults;

[numthreads(16, 16, 1)]
void CS_Fault(uint3 groupID : SV_GroupID, uint3 groupThreadID : SV_GroupThreadID, uint3 dispatchThreadID : SV_DispatchThreadID)
{
	uint cnt;
	faults.GetDimensions(cnt, 0);
	for(int i = 0; i < cnt; ++i)
	{
		if((dispatchThreadID.y > gradient * dispatchThreadID.x + offset) == gt)
			tex[dispatchThreadID.xy] +=  1.0 / (120 + i);
	}
}