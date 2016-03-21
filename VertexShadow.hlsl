cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
}

cbuffer Lights : register (b1)
{
	float3 position;
	matrix view;
	matrix projection;
	float4 ambient;
	float4 diffuse;
}

struct VS_IN
{
	float3 pos : POSITION;
	float2 depth : TEXCOORD;
	float3 norm : NORMAL;
};

struct VS_OUT
{
	float4 pos : SV_POSITION;
};

VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	output.pos = float4(input.pos, 1);

	output.pos = mul(output.pos, worldMatrix);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	return output;
}
