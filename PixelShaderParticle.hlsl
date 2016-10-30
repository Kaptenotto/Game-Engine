struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 size: SIZE;
	float3 color : COLOR;
};


float4 main(VS_OUT input) : SV_TARGET
{
	// we just send the color 
	return float4(input.color, 1.0f);
}