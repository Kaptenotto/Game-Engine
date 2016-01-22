Texture2D shaderTexture[2];
SamplerState SampleType;

cbuffer lightBuffer
{
	float4 diffuseColor;
	float3 lightDirection;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;

};


float4 main() : SV_TARGET
{
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}