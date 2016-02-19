struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 depth : TEXCOORD;
	float3 normal : NORMAL;
};

struct GSINPUT
{
	float4 pos : SV_POSITION;
	float2 depth : TEXCOORD;
	float4 norm : NORMAL;
};

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

[maxvertexcount(6)]
void GS_main(
	triangle GSINPUT input[3],
	inout TriangleStream <GSOutput> TriStream)
{
	GSOutput output = (GSOutput)0;

	matrix allMatrices = mul(mul(worldMatrix, view), projection);
	//matrix allMatrices = mul(view, projection);

	
	float3 faceEdgeA = input[1].pos - input[0].pos;
	float3 faceEdgeB = input[2].pos - input[0].pos;
	float3 normal = normalize(cross(faceEdgeA, faceEdgeB));

	for (int i = 0; i < 3; i++)
	{
		output.pos = mul(input[i].pos, allMatrices);

		output.depth = input[i].depth;
		output.normal = input[i].norm;

		TriStream.Append(output);
	}

}