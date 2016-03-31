SamplerState SampleTypeClamp : register(s0) ;

Texture2D reflectionTexture : register(t0);
Texture2D refractionTexture : register(t3);
Texture2D normalTexture : register(t2);

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
	//float clip : SV_ClipDistance0;
};
float4 PS_main(PS_IN input) : SV_TARGET
{

	float2 reflectTexCoord;	
	float2 refractTexCoord;

	float4 normalMap;
	float3 normal;
	float4 reflectionColor;
	float4 refractionColor ;
	float4 color;

	input.tex.y += waterTranslation;

	reflectTexCoord.x = input.reflectionPosition.x / input.reflectionPosition.w / 2.0f + 0.5f;
	reflectTexCoord.y = -input.reflectionPosition.y / input.reflectionPosition.w / 2.0f + 0.5f;

	refractTexCoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
	refractTexCoord.y = -input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;

	normalMap = normalTexture.Sample(SampleTypeClamp, input.tex);

	normal = (normalMap.xyz * 2.0f) - 1.0f;

	reflectTexCoord = reflectTexCoord + (normal.xy * reflectRefractScale);
	refractTexCoord = refractTexCoord + (normal.xy * reflectRefractScale);

	refractionColor = reflectionTexture.Sample(SampleTypeClamp, reflectTexCoord);
	reflectionColor = refractionTexture.Sample(SampleTypeClamp, refractTexCoord);

	color = lerp(reflectionColor, refractionColor, 0.6f);

	return color;
}