Texture2D txDiffuse : register (t0);
SamplerState sampAni;

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 normal : NORMAL;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	
	float3 s = txDiffuse.Sample(sampAni, input.uvs).xyz;

	return float4(input.uvs,1.0f, 1.0f);
};