cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
	float4 camPos;
}

struct GSInput
{
	float4 pos : SV_POSITION;
	float2 size: SIZE;
	float3 color : COLOR;
};

struct GSOutput
{
	float4 pos : SV_POSITION;
	float2 size: SIZE;
	float3 color : COLOR;
	float4 norm : NORMAL;
	uint primID : SV_PrimitiveID;
};

[maxvertexcount(4)]
void main(
	uint primID : SV_PrimitiveID,
	point GSInput input[1], 
	inout TriangleStream< GSOutput > output
)
{	
	float3 up = float3(0.0f, 1.0f, 0.0f);
	float3 look = camPos - input[0].pos;
	look.y = 0.0f; // y-axis aligned, so project to xz-plane  
	look = normalize(look); // we then normalize look  
	float3 right = cross(up, look); // then we calculate the x product = a new vector 

	float halfWidth = 0.5f*input[0].size.x; // half of the width from the input. 
	float halfHeight = 0.5f*input[0].size.y; // half of the height from the input we do this since we want the acutal center of the billboard


	
	float4 v[4]; // this is how we create the billboard from the information calculated this creates the billboarded "quad" 
	v[0] = float4(input[0].pos + halfWidth*right - halfHeight*up, 1.0f);
	v[1] = float4(input[0].pos + halfWidth*right + halfHeight*up, 1.0f);
	v[2] = float4(input[0].pos - halfWidth*right - halfHeight*up, 1.0f);
	v[3] = float4(input[0].pos - halfWidth*right + halfHeight*up, 1.0f);
	GSOutput element = (GSOutput)0;
	for (uint i = 0; i < 4; i++)
	{
		float4 p = mul(v[i], worldMatrix); // we first change the vertex into world,
		p = mul(p, camView); // then we translate it into view space
		p = mul(p, projectionMatrix); // and lastly we get it into projection view. 
		element.norm = float4(look,0.1); // we dont use this.
		element.pos = p;
		element.color = input[0].color;
		element.primID = primID;
		output.Append(element);
	}
}