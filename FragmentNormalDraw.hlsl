Texture2D shaderTexture : register(t0);
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
	float4 norm : NORMAL;
	float4 wPos : WPOS;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};
float4 PS_main(VS_OUT input) : SV_Target
{
	//Normal stuff
	float4 texColor;
	float4 norMap;
	float3 Normal;
	float3 lightDirection;
	float4 norColor;
	float lightIntensity;
	float4 color;
	float4 textureColor;
	color = float4(1, 1, 1, 1);

	norMap = shaderTexture.Sample(SampleTypeClamp, input.uvs);

	norMap = (norMap*2.0f) - 1.0f;

	norMap.z = (norMap.z * -1);

	Normal = (norMap.x * input.tangent) + (norMap.y * input.binormal) + (norMap.z * input.norm);
	Normal = normalize(Normal);

	lightDirection = (float4(-lightDir, 1) - input.wPos);

	lightIntensity  = saturate(dot(Normal, lightDirection));

	norColor = saturate(norMap * lightIntensity);

	return norColor;
};