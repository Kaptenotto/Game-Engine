struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 normal : NORMAL;
	//float3 lightPos : LIGHTPOSITION;
};

struct GSINPUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
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
	float4 temp = 0.0f;
	float4 worldPosition;
	float2 tcV1, tcV2, tcU1, tcU2;

	GSOutput output = (GSOutput)0;


	matrix allMatrices = mul(mul(worldMatrix, camView), projectionMatrix);

	int i;
	
	float3 faceEdgeA = input[1].pos - input[0].pos;
	float3 faceEdgeB = input[2].pos - input[0].pos;
	float3 normal = normalize(cross(faceEdgeA, faceEdgeB));

	float3 tangent = (tcV1 * faceEdgeA - tcV2 * faceEdgeB) * 1.0f / (tcU1 * tcV2 - tcU2 * tcV1);

	for (i = 0; i < 3; i++)
	{
		output.pos = mul(input[i].pos, allMatrices);
		temp = input[i].norm + temp;

		

		output.uvs = input[i].uvs;
		output.normal = input[i].norm; //mul(float4 (normal, 0), worldMatrix).xyz; // normal calculation

		//worldPosition = mul(input[i].pos, worldMatrix); // calculating worldposition with multiplying pos with worldmatrix
															// a float3, minus the worldposition calculated previousl
		
		TriStream.Append(output);
	}

}