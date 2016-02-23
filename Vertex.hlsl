cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
}

cbuffer Lights : register (b1)
{
	float4 position;
	matrix view;
	matrix projection;
	float4 ambient;
	float4 diffuse;
	float3 lightDir;
}

struct VS_IN
{
	float3 pos : POSITION;
	float2 uvs : TEXCOORD;
	float3 norm : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 wPos : WPOS;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.uvs = input.uvs;
	output.norm = float4(input.norm,1);
	output.pos = float4(input.pos, 1);

	output.pos = mul(output.pos, worldMatrix);
	output.wPos = output.pos;
	output.pos = mul(output.pos, camView);
	output.pos = mul(output.pos, projectionMatrix);

	return output;
}