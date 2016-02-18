struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 depth : TEXCOORD;
	float4 normal : NORMAL;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	return float4(input.depth, 0, 1);
	
	
};