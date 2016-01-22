Texture2D txDiffuse : register (t0);
SamplerState sampAni;

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 Color : COLOR;
	float3 normal : NORMAL;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	
	float3 s = txDiffuse.Sample(sampAni, input.Color).xyz;

	return float4(input.Color, 1.0f);
};