cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
	float4 camPos;

}

struct Input
{
	float3 worldPos		 : POSITION;
	float3 direction	 : DIRECTION;
	float3 color		 : COLOR;
	float  height		 : HEIGHT;
	float  width		 : WIDTH;
};

struct Output
{
	float4 worldPos		 : SV_POSITION;
	float3 direction	 : DIRECTION;
	float3 color		 : COLOR;
	float  height		 : HEIGHT;
	float  width		 : WIDTH;
};


Output main(Input input)
{
	Output output; // = (Output)0;
	
	output.worldPos = float4(input.worldPos, 1.0f);
	output.direction = input.direction;
	output.color = input.color;
	output.width = input.width;
	output.height = input.height;

	return output;
}