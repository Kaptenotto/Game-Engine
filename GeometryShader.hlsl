struct GSOutput
{
	float4 pos : SV_POSITION;
	float3 Color : COLOR;
	float3 normal : NORMAL;
};

struct GSINPUT
{
	float4 pos : SV_POSITION;
	float3 Color : COLOR;
};

cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
}

[maxvertexcount(3)]
void main(
	triangle GSINPUT input[3], 
	inout TriangleStream< GSOutput > output)
{
	GSOutput element = (GSOutput)0;

	matrix allMatrices = mul(mul(worldMatrix, viewMatrix), projectionMatrix);

	element.pos = mul(element.pos, allMatrices);

	float3 faceEdgeA = input[1].pos - input[0].pos;
	float3 faceEdgeB = input[2].pos - input[0].pos;
	float3 normal = normalize(cross(faceEdgeA, faceEdgeB));

	for (uint i = 0; i < 3; i++)
	{
		element.pos = mul(input[i].pos, allMatrices);
		element.Color = input[i].Color;

		output.Append(element);
	}
}