//Texture2D depthMapTexture : register(t1);
//SamplerState sampAni;

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 normal : NORMAL;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	


	return float4(input.uvs,0,1);
	
	
};