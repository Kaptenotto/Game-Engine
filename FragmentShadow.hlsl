//Texture2D depthMapTexture : register(t1);
//SamplerState sampAni;


cbuffer Lights : register (b1)
{
	float3 position;
	matrix view;
	matrix projection;
	float4 ambient;
	float4 diffuse;
}

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 normal : NORMAL;
	//float4 lightPos : POSITION;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	


	return input.pos;
	
	
};