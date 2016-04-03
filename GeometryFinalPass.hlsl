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
	float4 camPos;
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
		output.norm = input[i].norm;
		TriStream.Append(output);
	}


}