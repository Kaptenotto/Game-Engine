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
	float2 depth: TEXCOORD;
	float4 norm : NORMAL;
};

//-----------------------------------------------------------------------------------------
// VertexShader: VSScene
//-----------------------------------------------------------------------------------------
VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	//matrix cMatrix = mul(worldMatrix, mul(view, projection));
	//output.pos = mul(input.pos, cMatrix);
	output.pos = float4(input.pos, 1);
	output.depth = input.depth;
	output.norm = float4(input.norm, 1.0f);
	//output.depth.x = 1 - (output.pos.z / output.pos.w);

	return output;
}

//VS_OUT VS_main(VS_IN input)
//{
//	VS_OUT output = (VS_OUT)0;
//
//	matrix cMatrix = mul(mul(worldMatrix, view), projection);
//	output.pos = mul(input.pos, cMatrix);
//
//	output.uvs = input.uvs;
//	output.norm = float4(input.norm, 1);
//	//output.lightPos = position; //COORDS FOR LIGHT IN WORLD SPACE HERE?
//
//	return output;
//}