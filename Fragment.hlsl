Texture2D txDiffuse : register (t0);
Texture2D depthMapTexture : register(t1);
SamplerState sampAni  : register(s0);
SamplerState SampleTypeClamp : register(s1)
{
	Filter = MIN_MAG_MIP_POINT;
	AddressU = Clamp;
	AddressV = Clamp;
};

cbuffer light : register (b0)
{
	float4 ambient;
	float4 diffuse;
}

struct VS_OUT
{
	float4 pos : SV_POSITION;
	float3 Color : COLOR;
	float3 normal : NORMAL;
	float4 lightPos : TEXCOORD3;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;

	bias = 0.001f;
	color = ambient;
	projectTexCoord.x = input.lightPos.x / input.lightPos.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.lightPos.y / input.lightPos.w / 2.0f + 0.5f;

	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord.r);
		lightDepthValue = input.lightPos.z / input.lightPos.w;
		lightDepthValue = lightDepthValue - bias;

		if (lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(input.normal, input.lightPos)); //kan behöva ny tex (lightpos sepparat från poslightH
			if (lightIntensity > 0.0f)
			{
				color += (diffuse * lightIntensity);
				color = saturate(color);
			}
		}
	}

	//textureColor = txDiffuse.Sample(sampAni, input.Color);
	color = color * textureColor;

	return depthValue;


	//float3 s = txDiffuse.Sample(sampAni, input.Color).xyz;

	return float4(input.Color, 1.0f);
};