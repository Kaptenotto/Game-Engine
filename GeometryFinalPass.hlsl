struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
};

struct GSINPUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
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

	for (int i = 0; i < 3; i++)
	{
		output.pos = input[i].pos;

		output.uvs = input[i].uvs;
		output.norm = mul(input[i].norm, worldMatrix);

		output.tangent = tangent;

		output.binormal = binormal;

		output.wPos = input[i].wPos;

		TriStream.Append(output);
	}

}