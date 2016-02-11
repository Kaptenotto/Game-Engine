

struct Light
{
	float3 pos;
	float range;
	float3 dir;
	float cone;
	float3 att;
	float4 ambient;
	float4 diffuse;
};
cbuffer cbPerFrame
{
	Light light;
};
cbuffer cbPerObject
{
	float4x4 WVP;
	float4x4 World;
	float difColor;
	bool hasTexture;
	bool hasNormMap;

};

texture2D ObjTexture;
texture2D objNormMap;
SamplerState ObjSamplerState;
//TextureCube SkyMap;

struct VS_IN
{
	float4 pos : POSITION;
	float2 uvs : TEXCOORD;
	float3 norm : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 worldPos : POSITION;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = float4(input.pos, 1);

	//output.pos = mul(input.pos, MVP);
	//output.worldPos = mul(input.pos World);
	//output.tangent = mul(tangent, World);

	//output.Color = float3 (1.0f,1.0f,1.0f);

	output.uvs = input.uvs;
	output.norm = float4(input.norm,1);


	return output;
}