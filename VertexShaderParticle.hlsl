cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
}

struct Input
{
	float3 position : POSITION;
	float3 color : COLOR;
};

struct Output
{
	float4 position : SV_POSITION;
	float3 color : COLOR;
};


float4 main(Input input)
{
	Output output = (Output)0;
	
	output.position = input.position;
	output.color = input.color;

	return output;
}