cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

cbuffer ClipPlaneBuffer
{
	float4 clipPlane;

};

struct VS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
};

struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	//float clipPlane : SV_ClipDistance0;
};


VS_OUT VS_main(VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	input.position.w = 1.0f;

	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;
	output.normal = mul(input.normal,worldMatrix);

	output.normal = normalize(output.normal);

	//output.clip = dot(mul(input.position, worldMatrix), clipPlane);

	return output;
}