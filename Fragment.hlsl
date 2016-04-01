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
	color = float4(1,1,1,1);
	textureColor = TextureRes.Sample(SampleTypeClamp, input.uvs);

	return textureColor;
};
