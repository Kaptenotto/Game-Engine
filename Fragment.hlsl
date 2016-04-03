Texture2D TextureRes : register (t0);
Texture2D ShadowRes : register (t1);
Texture2D NormalRes : register (t2);
SamplerState SampleTypeClamp : register(s0);

cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
	float4 camPos;
}
cbuffer Lights : register (b1)
{
	float4 position;
	matrix view;
	matrix projection;
	float4 ambient;
	float4 diffuse;
	float3 lightDir;
}

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float3 norm : NORMAL;
};

float4 PS_main(VS_OUT input) : SV_Target
{	
		float4 color;
		float4 textureColor;
		float4 shadowColor;
		float4 normalColor;
		float4 finalColor;
		color = ambient;
		textureColor = TextureRes.Sample(SampleTypeClamp, input.uvs);
		shadowColor = ShadowRes.Sample(SampleTypeClamp, input.uvs);
		normalColor = NormalRes.Sample(SampleTypeClamp, input.uvs);
	
		textureColor.a = 1;
		shadowColor.a = 1;
		normalColor.a = 1;
	
	
		finalColor = ambient;
		finalColor = finalColor * textureColor + finalColor * shadowColor + finalColor * normalColor;
		//finalColor = finalColor * shadowColor;
		//finalColor = finalColor * normalColor;
		//finalColor = finalColor * ambient;
		finalColor = saturate(finalColor);
		return finalColor;
};



//float4 PS_main(VS_OUT input) : SV_Target
//{
//	float4 color;
//	float4 textureColor;
//	float4 shadowColor;
//	float4 normalColor;
//	float4 finalColor;
//	color = ambient;
//	textureColor = TextureRes.Sample(SampleTypeClamp, input.uvs);
//	shadowColor = ShadowRes.Sample(SampleTypeClamp, input.uvs);
//	normalColor = NormalRes.Sample(SampleTypeClamp, input.uvs);
//
//	textureColor.a = 1;
//	shadowColor.a = 1;
//	normalColor.a = 1;
//
//
//	finalColor = ambient;
//	finalColor = finalColor * textureColor + finalColor * shadowColor + finalColor * normalColor;
//	//finalColor = finalColor * shadowColor;
//	//finalColor = finalColor * normalColor;
//	//finalColor = finalColor * ambient;
//	finalColor = saturate(finalColor);
//	return finalColor;
//};
