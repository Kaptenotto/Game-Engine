cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
	matrix lightViewMatrix;
	matrix lightProjectionMatrix;

};
struct VSShadow_IN
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

struct VSShadow_OUT
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};

VSShadow_OUT VSShadow_main(VSShadow_IN input)
{
	VSShadow_OUT output = (VSShadow_OUT)0;

	float4 WorldPosition;

	matrix cMatrix = mul(WorldMatrix, mul(viewMatrix, projectionMatrix));	//Combination Matrix
	output.position = mul(input.position, cMatrix);							//mulligan pos
	output.lightViewPosition = mul(input.position, cMatrix);				//mulligan lightviewpos
	output.tex = input.tex;													//Tex coord for pixel
	output.normal = mul(input.normal, (float3x3)worldMatrix);				//norm vs worldmatrix
	output.normal = normalize(output.normal);								//normalize
	worldPosition = mul(input.position, worldMatrix);						//vert pos in world
	output.lightPos = lightPosition.xyz - worldPosition.xyz;				//light pos based on pos of light and pos of vert in world
	output.lightPos = normalize(output.lightPos);							//normalize

	return output;
}