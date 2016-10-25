struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 wPos : WPOS;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
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
	float4 camPos;

}

[maxvertexcount(6)]
void main(
	triangle GSINPUT input[3],
	inout TriangleStream <GSOutput> TriStream)
{
	GSOutput output = (GSOutput)0;

	float3 faceEdgeA = input[1].pos - input[0].pos;
	float3 faceEdgeB = input[2].pos - input[0].pos;
	float2 uvEdge1 = input[1].uvs - input[0].uvs;
	float2 uvEdge2 = input[2].uvs - input[0].uvs;

	float3 normal = normalize(-cross(faceEdgeA, faceEdgeB));
	float3 tangent = (uvEdge2[1] * faceEdgeA - uvEdge1[1] * faceEdgeB)*(1 / (uvEdge1[0] * uvEdge2[1] - uvEdge2[0] * uvEdge1[1]));
	tangent = normalize(tangent);

	float3 binormal = normalize(-cross(normal, tangent));

	tangent = mul(float4(tangent, 1), worldMatrix).xyz;
	binormal = mul(float4(binormal, 1), worldMatrix).xyz;
	
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
