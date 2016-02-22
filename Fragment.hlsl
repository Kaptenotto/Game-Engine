Texture2D txDiffuse : register (t0);
Texture2D depthMapTexture : register(t1);
SamplerState SampleTypeClamp : register(s0)
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};
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
}

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float2 uvs : TEXCOORD;
	float4 norm : NORMAL;
	float4 wPos : WPOS;
};

//float4 PS_main(VS_OUT input) : SV_Target
//{
//	float bias;
//	float4 color;
//	float2 projectTexCoord;
//	float depthValue;
//	float lightDepthValue;
//	float lightIntensity;
//	float4 textureColor;
//	float4 lightPos;
//
//	bias = 0.00001f;
//	color = ambient;
//
//	lightPos = mul(input.wPos, view);
//	lightPos = mul(lightPos, projection);
//
//	projectTexCoord.x = lightPos.x / lightPos.w;
//	projectTexCoord.y = lightPos.y / lightPos.w;
//	
//	lightDepthValue = lightPos.z / lightPos.w;
//	
//	projectTexCoord.x = projectTexCoord.x * 0.5f + 0.5f;
//	projectTexCoord.y = projectTexCoord.y * -0.5f + 0.5f;
//
//	depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord.xy).r + bias;
//
//	textureColor = txDiffuse.Sample(SampleTypeClamp, input.uvs);
//
//	float dx = 1.0f / SMAP_SIZE;
//	float s0 = (depthMapTexture.Sample(SampleTypeClamp, projectTexCoord).r + bias < lightDepthValue) ? 0.0f : 1.0f;
//	float s1 = (depthMapTexture.Sample(SampleTypeClamp, projectTexCoord + float2(dx, 0.0f)).r + bias < lightDepthValue) ? 0.0f : 1.0f;
//	float s2 = (depthMapTexture.Sample(SampleTypeClamp, projectTexCoord + float2(0.0f, dx)).r + bias < lightDepthValue) ? 0.0f : 1.0f;
//	float s3 = (depthMapTexture.Sample(SampleTypeClamp, projectTexCoord + float2(dx, dx)).r + bias < lightDepthValue) ? 0.0f : 1.0f;
//
//	float2 texelpos = projectTexCoord * SMAP_SIZE;
//	float2 lerps = frac(texelpos);
//	float shadowcooef = lerp(lerp(s0, s1, lerps.x), lerp(s2, s3, lerp.x), lerps.y);
//	float3 litcolor = texturecolor.rgb * shadowcooef;
//	return float4(litcolor, 1);
//
//
//	color = color * textureColor;
//
//	return color;
//};

float4 PS_main(VS_OUT input) : SV_Target
{
	float bias;
float4 color;
float2 projectTexCoord;
float depthValue;
float lightDepthValue;
float lightIntensity;
float4 textureColor;
float4 lightPos;

bias = 0.00001f;
color = ambient;

lightPos = mul(input.wPos, view);
lightPos = mul(lightPos, projection);

projectTexCoord.x = lightPos.x / lightPos.w;
projectTexCoord.y = lightPos.y / lightPos.w;

lightDepthValue = lightPos.z / lightPos.w;

projectTexCoord.x = projectTexCoord.x * 0.5f + 0.5f;
projectTexCoord.y = projectTexCoord.y * -0.5f + 0.5f;

depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord.xy).r + bias;

if (depthValue < lightDepthValue)

{
	lightIntensity = saturate(dot(input.norm, position));
	if (lightIntensity > 0.0f)
	{
		color += (diffuse * lightIntensity);
		color = saturate(color);
	}
}

textureColor = txDiffuse.Sample(SampleTypeClamp, input.uvs);
color = color * textureColor;

return color;
};