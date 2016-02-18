struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 lightPos : POSITION;
};

struct GSINPUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 lightPos : POSITION;
};

cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
}

[maxvertexcount(6)]
void main(
	triangle GSINPUT input[3],
	inout TriangleStream <GSOutput> TriStream)
{

	float4 worldPosition;

	GSOutput output = (GSOutput)0;

	matrix allMatrices = mul(mul(worldMatrix, camView), projectionMatrix);


	
	float3 faceEdgeA = input[1].pos - input[0].pos;
	float3 faceEdgeB = input[2].pos - input[0].pos;
	float3 normal = normalize(cross(faceEdgeA, faceEdgeB));

	for (int i = 0; i < 3; i++)
	{
		output.pos = mul(input[i].pos, allMatrices);

		output.uvs = input[i].uvs;
		output.lightPos = input[i].lightPos;
		output.norm = input[i].norm; //mul(float4 (normal, 0), worldMatrix).xyz; // normal calculation

		//worldPosition = mul(input[i].pos, worldMatrix); // calculating worldposition with multiplying pos with worldmatrix
															// a float3, minus the worldposition calculated previously
		TriStream.Append(output);
	}

}