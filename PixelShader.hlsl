Texture2D shaderTexture[2];
SamplerState SampleType;

cbuffer lightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
};
cbuffer lightBuffer2
{
	float3 lightPosition;
	float padding;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float4 lightViewPosition : TEXCOORD1;
	float3 lightPos : TEXCOORD2;
};


float4 main() : SV_TARGET
{
	//return float4(1.0f, 1.0f, 1.0f, 1.0f);


}