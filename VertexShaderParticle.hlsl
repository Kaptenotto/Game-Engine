cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
	float4 camPos;

}

struct Input
{
	float3 position : POSITION;
	float2 size: SIZE;
	float3 color : COLOR;
};

struct Output
{
	float4 position : SV_POSITION;
	float2 size: SIZE;
	float3 color : COLOR;
};


Output main(Input input)
{
	Output output = (Output)0;
	
	output.position = float4(input.position,1.0);
	output.size = input.size;
	output.color = input.color;

	return output;
}