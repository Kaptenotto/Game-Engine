struct VSShadow_IN
{
	float3 pos : POSITION;
	float3 Color : COLOR;
};

struct VSShadow_OUT
{
	float4 pos : SV_POSITION;
	float3 Color : COLOR;
};

VSShadow_OUT VSShadow_main(VSShadow_IN input)
{
	VSShadow_OUT output = (VSShadow_OUT)0;

	output.pos = float4(input.pos, 1);
	output.Color = input.Color;

	return output;
}