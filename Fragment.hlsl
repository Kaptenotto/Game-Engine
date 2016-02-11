Texture2D txDiffuse : register (t0);
Texture2D depthMapTexture : register(t1);
SamplerState SampleTypeClamp : register(s0);
SamplerState SampleTypeWrap  : register(s1);

struct VS_OUT
{
	float3 pos : POSITION;
	float3 norm : TEXCOORD1;
	float2 tex : COLOR; //ska va tex
	float4 posLightH : TEXCOORD3;
};
struct Light
{
	float4 ambient;
	float4 diffuse;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	Light light = (Light)0;

	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;

	bias = 0.001f;
	color = light.ambient;
	projectTexCoord.x = input.posLightH.x / input.posLightH.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.posLightH.y / input.posLightH.w / 2.0f + 0.5f;

	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord.r);
		lightDepthValue = input.posLightH.z / input.posLightH.w;
		lightDepthValue = lightDepthValue - bias;

		if (lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(input.norm, input.posLightH)); //kan behöva ny tex (lightpos sepparat från poslightH
			if (lightIntensity > 0.0f)
			{
				color += (light.diffuse * lightIntensity);
				color = saturate(color);
			}
		}
	}

	textureColor = txDiffuse.Sample(SampleTypeWrap, input.tex);
	color = color * textureColor;

	return color;

	//float3 s = txDiffuse.Sample(sampAni, input.Color).xyz;

	//return float4(input.Color, 1.0f);
};