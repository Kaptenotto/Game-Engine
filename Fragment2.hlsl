Texture2D shaderTexture : register(t0);
Texture2D depthMapTexture : register(t1);
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);

cbuffer LightBuffer
{
	float4 ambientColor;
	float4 diffuseColor;
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

struct PS_OUT
{
	float4 pos : SV_POSITION;
	float3 Color : COLOR;
	float3 normal : NORMAL;
};


float4 PS_main(PS_OUT input) : SV_Target
{
	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;

	//bias = 0.001f;
	//color = ambientColor;
	//projectTexCoord.x = input.lightViewPosition.x / input.lightViewPosition.w / 2.0f + 0.5f;
	//projectTexCoord.y = -input.lightViewPosition.y / input.lightViewPosition.w / 2.0f + 0.5f;
	//
	//if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	//{
	//	depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord.r);
	//	lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
	//	lightDepthValue = lightDepthValue - bias;

	//	if (lightDepthValue < depthValue)
	//	{
	//		lightIntensity = saturate(dot(input.normal, input.lightPos));
	//		if (lightIntensity > 0.0f)
	//		{
	//			color += (diffuseColor * lightIntensity);
	//			color = saturate(color);
	//		}
	//	}
	//}

	//textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);
	//color = color * textureColor;

	return input.color;
};