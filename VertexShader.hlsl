cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;

};
cbuffer ReflectionBuffer
{
	matrix reflection;
};

struct VS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 reflection : TEXCOORD1;
	float4 refraction : TEXCOORD2;
	//float clip : SV_ClipDistance0;
};
struct VS_OUT
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 reflection : TEXCOORD1;
	float4 refraction : TEXCOORD2;
	//float clip : SV_ClipDistance0;
};


 VS_OUT VS_main( VS_IN input)
{
	VS_OUT output = (VS_OUT)0;

	matrix reflectProjectWorld;
	matrix viewProjectWorld;

	input.position.w = 1.0f;
	
	output.position = mul(input.position, worldMatrix);
	output.position = mul(output.position, viewMatrix);
	output.position = mul(output.position, projectionMatrix);

	output.tex = input.tex;

	reflectProjectWorld = mul(reflection, projectionMatrix);
	reflectProjectWorld = mul(worldMatrix, reflectProjectWorld);

	output.reflection = mul(input.position, reflectProjectWorld);
	
	viewProjectWorld = mul(viewMatrix, projectionMatrix);
	viewProjectWorld = mul(worldMatrix, viewProjectWorld);

	output.refraction = mul(input.position, viewProjectWorld);

	//output.clip = dot(mul(input.position, worldMatrix)clipPlane);

	return output;
}