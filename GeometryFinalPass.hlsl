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
		output.pos = mul(input[i].pos, camView);
		output.uvs = input[i].uvs;
		output.norm = camPos - input[i].norm;
		output.norm = normalize(output.norm);
		TriStream.Append(output);
	}


}