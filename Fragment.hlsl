Texture2D txDiffuse : register (t0);
SamplerState sampAni;

struct VS_OUT
{
	float3 pos : POSITION;
	float3 norm : NORMAL;
	float2 tex : TEXCOORD0;
	float4 posLightH : TEXCOORD3;
};

float4 PS_main(VS_OUT input) : SV_Target
{
	float4 texColor = diffuseMap.Sample(textureSampler, input.tex)

	float bias;
	float4 color;
	float2 projectTexCoord;
	float depthValue;
	float lightDepthValue;
	float lightIntensity;
	float4 textureColor;

	bias = 0.001f;
	color = ambientColor;
	projectTexCoord.x = input.posLightH.x / input.posLightH.w / 2.0f + 0.5f;
	projectTexCoord.y = -input.posLightH.y / input.posLightH.w / 2.0f + 0.5f;

	if ((saturate(projectTexCoord.x) == projectTexCoord.x) && (saturate(projectTexCoord.y) == projectTexCoord.y))
	{
		depthValue = depthMapTexture.Sample(SampleTypeClamp, projectTexCoord.r);
		lightDepthValue = input.lightViewPosition.z / input.lightViewPosition.w;
		lightDepthValue = lightDepthValue - bias;

		if (lightDepthValue < depthValue)
		{
			lightIntensity = saturate(dot(input.normal, input.lightPos));
			if (lightIntensity > 0.0f)
			{
				color += (diffuseColor * lightIntensity);
				color = saturate(color);
			}
		}
	}

	textureColor = shaderTexture.Sample(SampleTypeWrap, input.tex);
	color = color * textureColor;

	return input.color;

	//float3 s = txDiffuse.Sample(sampAni, input.Color).xyz;

	//return float4(input.Color, 1.0f);
};