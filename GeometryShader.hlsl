struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 wPos : WPOS;
};

struct GSINPUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 wPos : WPOS;
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
	GSOutput output = (GSOutput)0;

	matrix allMatrices = mul(mul(worldMatrix, camView), projectionMatrix);
	
	float3 faceEdgeA = input[1].pos - input[0].pos;
	float3 faceEdgeB = input[2].pos - input[0].pos;
	float3 normal = normalize(cross(faceEdgeA, faceEdgeB));

	for (int i = 0; i < 3; i++)
	{
		output.pos = input[i].pos;

		output.uvs = input[i].uvs;
		output.norm = float4(normal, 0); //mul(float4 (normal, 0), worldMatrix).xyz; // normal calculation
		//output.norm = input[i].norm;
		output.wPos = input[i].wPos;

		TriStream.Append(output);
	}

}