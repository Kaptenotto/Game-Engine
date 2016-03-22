Texture2D TextureMap : register (t0);
Texture2D ShadowMap : register(t1);
Texture2D NormalMap : register(t2);
SamplerState SampleTypeClamp : register(s0);

cbuffer MatrixBuffer : register (b0)
{
	matrix worldMatrix;
	matrix camView;
	matrix projectionMatrix;
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
	float4 norm : NORMAL;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	float4 textureColor;
	float4 color;
	color = ambient;
	textureColor = txDiffuse.Sample(SampleTypeClamp, input.uvs);
	textureColor = textureColor * color;
	return textureColor;
};
