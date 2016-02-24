SamplerState SampleType;

Texture2D reflectionTexture;
Texture2D refractionTexture;
Texture2D normalTexture;

cbuffer WaterBuffer
{
	float waterTranslation;
	float reflectRefractScale;
	float2 padding;
};

struct PS_IN
{
	float4 position : SV_POSITION;
	float2 tex : TEXCOORD;
	float4 reflectionPosition : TEXCOORD1;
	float4 refractionPosition : TEXCOORD2;
};


float4 PS_main(PS_IN input) : SV_TARGET
{

	float2 reflectTexCoord;	
	float2 refractTexCoord;

	float4 normalMap;
	float3 normal;
	float4 reflectionColor;
	float4 refractionColor;
	float4 color;

	input.tex.y += waterTranslation;

	reflectTexCoord.x = input.reflectPosition.x / input.reflectionPosition.w / 2.0f + 0.5f

	return float4(1.0f, 1.0f, 1.0f, 1.0f);
}