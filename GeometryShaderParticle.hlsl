cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
	float4 camPos;

}

struct GSInput
{
	float4 worldPos		 : SV_POSITION;
	float3 direction	 : DIRECTION;
	float3 color		 : COLOR;
	float  height		 : HEIGHT;
	float  width		 : WIDTH;
};

struct GSOutput
{
	float4 Pos		 : SV_POSITION;
	float3 direction	 : DIRECTION;
	float3 color		 : COLOR;
	float  height		 : HEIGHT;
	float  width		 : WIDTH;
	float4 Normal : NORMAL;
	float4 wPos : WORLDPOS;
	float camPos : CAMERAPOS;
};

[maxvertexcount(4)]
void main(
	uint primID : SV_PrimitiveID,
	point GSInput input[1], 
	inout TriangleStream< GSOutput > output
)
{	


	/*float3 up = float3(0.0f, 1.0f, 0.0f);
	//float3 look = camPos - input[0].pos;
	//look.y = 0.0f; // y-axis aligned, so project to xz-plane
	//look = normalize(look);
	//float3 right = cross(up, look);

	//float halfWidth = 0.5f*input[0].size.x;
	//float halfHeight = 0.5f*input[0].size.y;*/
	float3 vecToCam = (input[0].worldPos.xyz - camPos.xyz);
	vecToCam.x = 0.0f;
	vecToCam = normalize(vecToCam);
	float3 upVec = normalize(input[0].direction);
	//upVec.y = 0;
	upVec = normalize(upVec);

	float3 rightVec = normalize(cross(-vecToCam, upVec));

	//Get vertices for the quad
	float3 vert[4];


	vert[0] = input[0].worldPos.xyz - rightVec * input[0].width - upVec * input[0].height;
	vert[1] = input[0].worldPos.xyz - rightVec * input[0].width + upVec * input[0].height;
	vert[2] = input[0].worldPos.xyz + rightVec * input[0].width - upVec * input[0].height;
	vert[3] = input[0].worldPos.xyz + rightVec * input[0].width + upVec * input[0].height;

	GSOutput element = (GSOutput)0;
	for (uint i = 0; i < 4; i++)
	{
		element.Pos = mul(mul(float4(vert[i], 1.0f), camView), projectionMatrix);
		element.Normal = (-vecToCam,1.0f);
		//element.pos = p;
		element.color = input[0].color;
		element.wPos = float4(vert[i], 1.0f);
		element.camPos = camPos;
		//element.primID = primID;
		output.Append(element);
	}
}