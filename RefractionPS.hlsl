Texture2D shaderTexture;
SamplerState SampleType;

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
	float3 lightDir;
};

struct PS_IN
{
	float4 position: SV_POSITION;
	float2 tex : TEXCOORD;
	float3 normal : NORMAL;
	//float clipPlane : SV_ClipDistance0;

};

float4 PS_main(PS_IN input) : SV_TARGET
{

	float4 textureColor;	
	float3 lightDirection;
	float lightInt;
	float4 color;

	textureColor = shaderTexture.Sample(SampleType, input.tex);

	color = ambientColor;
	lightDirection = -lightDir;

	lightInt = saturate(dot(input.normal, lightDirection));

	color = saturate(color);

	color = color * textureColor;



	return color;
}